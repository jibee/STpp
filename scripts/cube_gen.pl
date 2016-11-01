#!perl

use strict;
use warnings;
use utf8;
use feature qw(say state switch);
use feature qw(unicode_strings unicode_eval current_sub fc evalbytes);

use IO::File;
use Data::Dumper::Concise;
use String::LCSS;
use List::AllUtils qw/uniq/;


my $spec = parse_cube_conf($ARGV[0]) if @ARGV;

sub parse_cube_conf
{
    my ($fname)=@_;
    my $f = new IO::File($fname, "r") or die("Cannot open $fname for reading");
    my $general_spec= consume_header($f);
    my $peripherals_spec = consume_peripherals($f);
    my $pins_spec = consume_pins($f);

    $_->{"PINs"}=~s/\*$// foreach @$pins_spec;
    foreach(@$pins_spec)
    {
        $_->{"LABELs"}=$1 if $_->{"LABELs"}=~/\[([^\]]+)/;
    }
    my %pins = map { $_->{"PINs"}=>$_ } @$pins_spec;
    my %peripherals = map { $_->{"PERIPHERALS"}=>{} } @$peripherals_spec;
    my %peripheral_pins;
    foreach my $peripheral_spec (@$peripherals_spec)
    {
	my $pin = $peripheral_spec->{"PINS"};
	my $pin_spec = $pins{$pin};
	$peripherals{$peripheral_spec->{"PERIPHERALS"}}->{$peripheral_spec->{"FUNCTIONS"}}={%$peripheral_spec, %$pin_spec};
	$peripheral_pins{$pin}=$peripheral_spec->{"PERIPHERALS"};
    }
    my %unused_pins = map {$_=>$pins{$_}} grep {not $peripheral_pins{$_}} keys %pins;

    my ($header, $implementation)=issue_classes($general_spec, \%peripherals, \%unused_pins);
    my $hpp = new IO::File("../inc/".$general_spec->{Configuration}.".hpp", "w");
    my $cpp = new IO::File("../lib/".$general_spec->{Configuration}.".cpp", "w");
    print $hpp $header;
    print $cpp $implementation;
}

sub issue_classes
{
    my ($general_spec, $peripherals, $unused_pins)=@_;
    my $baseClass = "Platform::STM32F4";
    if(exists $peripherals->{USB_OTG_FS})
    {
	$baseClass = "Platform::STM32F4_WithUSB";
	delete $peripherals->{USB_OTG_FS};
    }
    delete $peripherals->{RCC};
    delete $peripherals->{SYS};
    my $def = {init=>[], decl=>[], cons=>[], includes=>[]};
    foreach my $pin_name (sort keys %$unused_pins)
    {
	my $pin_def = $unused_pins->{$pin_name};
	my $defined_name = $pin_def->{LABELs}||$pin_name;
	if($defined_name=~/\[([^\]]+)/)
	{
	warn("Replacing pin name $defined_name with $1");
            $defined_name = $1;
	}
	$defined_name=~s/(\[|\]|\s|\\|\/)/_/g;
	$defined_name=~s/_+/_/g;
	$defined_name=~s/^_//;
	$defined_name=~s/_$//;
	$defined_name=~s/-/_/g;
	process_gpio_decl($def, $pin_def, $defined_name);
    }


# Process ADC: simply set the relevant GPIOs into ADC mode
    my $issued_adcs={};
    my %adc;
    foreach (qw/ADC1 ADC2 ADC3/)
    {
	if($peripherals->{$_})
	{
            %adc = (%adc, %{$peripherals->{$_}});
            delete $peripherals->{$_};
	}
    }

    process_adc($def, \%adc, $issued_adcs);
# Process UART
    process_uart($def, $peripherals, $_) foreach (qw/UART1 UART2 UART3 UART4/);
# Process the Timers and the relevant PWMs
    process_timer($def, $peripherals, $_) foreach (map {"TIM".$_} (1..14));
# Process the SPI buses
    process_SPI($def, $peripherals, $_) foreach(qw/SPI1 SPI2 SPI3/);
# Process the I2C buses
    process_I2C($def, $peripherals, $_) foreach(qw/I2C1 I2C2 I2C3/);
# Process the I2S buses
#    process_I2S($def, $peripherals, $_) foreach(qw/I2S1 I2S2 I2S3/);
    say Dumper($peripherals);

    return code_gen($baseClass, $general_spec->{Configuration}, $def);
}

sub process_I2C
{
    my ($def, $periphs, $name) = @_;
    return unless $periphs->{$name};
    my $periph = $periphs->{$name};
    delete $periphs->{$name};
    
    my $original_name = $name;
    my @labels = grep {$_} uniq(map {$_->{LABELs}} values %$periph);
    if(@labels)
    {
	$name=common_part(@labels);
    }
    
# TODO determine if a DMA stream has been setup for this I2C port
    my $sda_pin;
    my $scl_pin;
    foreach my $x_pin (values %$periph)
    {
	my $pin_name = $x_pin->{FUNCTIONS};
	$pin_name=~s/I2C/$name/;
	$scl_pin=$pin_name if $pin_name=~/_SCL$/;
	$sda_pin=$pin_name if $pin_name=~/_SDA$/;
        process_gpio_decl($def, $x_pin, $pin_name);
    }
    
    push @{$def->{includes}}, "#include <I2C.hpp>";
    push @{$def->{decl}}, "Platform::I2C ".$name.";";
    push @{$def->{init}}, $name."(".$scl_pin.", ".$sda_pin.", Platform::I2C::I2C_".substr($original_name, -1, 1).")";
}
sub process_SPI
{
    my ($def, $periphs, $name) = @_;
    return unless $periphs->{$name};
    my $periph = $periphs->{$name};
    delete $periphs->{$name};
    
    my $original_name = $name;
    my @labels = grep {$_} uniq(map {$_->{LABELs}} values %$periph);
    if(@labels)
    {
	$name=common_part(@labels);
    }
    
# TODO determine if a DMA stream has been setup for this SPI port
    my $miso_pin;
    my $mosi_pin;
    my $clk_pin;
    foreach my $x_pin (values %$periph)
    {
	my $pin_name = $x_pin->{FUNCTIONS};
	$pin_name=~s/SPI/$name/;
	$mosi_pin=$pin_name if $pin_name=~/_MOSI$/;
	$miso_pin=$pin_name if $pin_name=~/_MISO$/;
	$clk_pin=$pin_name if $pin_name=~/_CLK$/;
        process_gpio_decl($def, $x_pin, $pin_name);
        push @{$def->{cons}}, $name.".configGpio(".$pin_name.");";
    }
    
    push @{$def->{includes}}, "#include <Spi.h>";
    push @{$def->{decl}}, "Platform::Spi ".$name.";";
    push @{$def->{init}}, $name."(".substr($original_name, -1, 1).")";
    
    #say Dumper($periph);
    
}

sub process_timer
{
    my ($def, $periphs, $name) = @_;
    return unless $periphs->{$name};
    my $periph = $periphs->{$name};
    delete $periphs->{$name};

    my @modes = map $_->{MODES}, (values %$periph);
    $_=~s/ CH[1-8]$//  foreach @modes;
    my %modes = map {$_=>1} @modes; 
    if($modes{"Encoder Mode"})
    {
	process_timer_encoder($def, $periph, $name);
	return;
    }
    if($modes{"PWM Generation"})
    {
	process_timer_pwm($def, $periph, $name);
	return;
    }

    say Dumper $periph;
    exit;
}

sub timer_type
{
    my ($periph)=@_;
    my $timer_name=$periph->{"PERIPHERALS"};
    my %timer_map = (
	    TIM1=>"AdvancedControlTimer",
	    TIM2=>"GeneralPurposeTimer<uint32_t,4>",
	    TIM3=>"GeneralPurposeTimer<uint16_t,4>",
	    TIM4=>"GeneralPurposeTimer<uint16_t,4>",
	    TIM5=>"GeneralPurposeTimer<uint32_t,4>",
	    TIM6=>"Timer",
	    TIM7=>"Timer",
	    TIM8=>"AdvancedControlTimer",
	    TIM9=>"GeneralPurposeTimer<uint16_t,2>",
	    TIM10=>"GeneralPurposeTimer<uint16_t,1>",
	    TIM11=>"GeneralPurposeTimer<uint16_t,1>",
	    TIM12=>"GeneralPurposeTimer<uint16_t,2>",
	    TIM13=>"GeneralPurposeTimer<uint16_t,1>",
	    TIM14=>"GeneralPurposeTimer<uint16_t,1>"
	    );
    my %channel_map = (
	    "AdvancedControlTimer"=>4,
	    "GeneralPurposeTimer<uint32_t,4>"=>4,
	    "GeneralPurposeTimer<uint16_t,4>"=>4,
	    "Timer"=>0,
	    "GeneralPurposeTimer<uint16_t,2>"=>2,
	    "GeneralPurposeTimer<uint16_t,1>"=>1
	    );
    my %size_map= (
	    "AdvancedControlTimer"=>16,
	    "GeneralPurposeTimer<uint32_t,4>"=>32,
	    "GeneralPurposeTimer<uint16_t,4>"=>16,
	    "Timer"=>0,
	    "GeneralPurposeTimer<uint16_t,2>"=>16,
	    "GeneralPurposeTimer<uint16_t,1>"=>16
	    );
    my $timer_type = $timer_map{$timer_name};
    my $channel_count = $channel_map{$timer_type};
    my $channel_width = $size_map{$timer_type};
    my $name = $timer_name;
    $name =~ s/TIM/Tim/;
    return {type=>$timer_type, count=>$channel_count, width=>$channel_width, name=>$name};
}

sub process_timer_encoder
{
    my ($def, $periph, $name) = @_;
    my $timer_spec = timer_type($periph->{$name."_CH1"});
    my $timer = $timer_spec->{name};
    my $encoder_type = "IncrementalEncoder16";
    if($timer_spec->{width}==32)
    {
	$encoder_type = "IncrementalEncoder32";
    }
    my $gpioa = $name."_CH1";
    $gpioa = $periph->{$gpioa}->{LABELs} if $periph->{$gpioa}->{LABELs};
    process_gpio_decl($def, $periph->{$name."_CH1"}, $gpioa);
    my $gpiob = $name."_CH2";
    $gpiob = $periph->{$gpiob}->{LABELs} if $periph->{$gpiob}->{LABELs};
    process_gpio_decl($def, $periph->{$name."_CH2"}, $gpiob);

    $name = common_part($gpioa, $gpiob);
    push @{$def->{includes}}, "#include <IncrementalEncoder.h>";
    push @{$def->{decl}}, $encoder_type." ".$name.";";
    push @{$def->{init}}, "$name($gpioa, $gpiob, $timer)";
}
sub process_timer_pwm
{
    my ($def, $periph, $name) = @_;
    foreach my $pwm (values %$periph)
    {
	my $timer_spec = timer_type($pwm);
	my $timer = $timer_spec->{name};
	my $pwm_type= "Pwm<Platform::".$timer_spec->{type}.">";
	my $pwm_name = "PWM_".$pwm->{PINS};
	$pwm_name = $pwm->{LABELs} if($pwm->{LABELs});
	my $gpio = $pwm_name."_IO";

	process_gpio_decl($def, $pwm, $gpio);
	my $channel = $pwm->{FUNCTIONS};
	if($channel =~/(\d)$/)
	{
	    $channel=$1;
	}
	push @{$def->{decl}}, $pwm_type." ".$pwm_name.";";
	push @{$def->{init}}, "$pwm_name($gpio, $timer, $channel)";
    }
    push @{$def->{includes}}, "#include <Pwm.h>";
}

sub process_uart
{
    my ($def, $periphs, $name) = @_;
    return unless $periphs->{$name};
    my $periph = $periphs->{$name};
    delete $periphs->{$name};
    my $original_name = $name;
    my @labels = grep {$_} uniq(map {$_->{LABELs}} values %$periph);
    if(@labels)
    {
	$name=common_part(@labels);
    }
    else
    {
	$name=~s/UART/Uart/;
    }


# TODO determine if a DMA stream has been setup for this UART
    my $tx_pin;
    my $rx_pin;
    foreach my $x_pin (values %$periph)
    {
	my $pin_name = $x_pin->{FUNCTIONS};
	$pin_name=~s/UART/$name/;
	if($pin_name=~/_(T|R)X$/)
	{
	    $tx_pin = $pin_name if($1 eq "T");
	    $rx_pin = $pin_name if($1 eq "R");
	    process_gpio_decl($def, $x_pin, $pin_name);
	}
    }
    push @{$def->{includes}}, "#include <Uart.h>";
    push @{$def->{decl}}, "Platform::Uart ".$name.";";
    push @{$def->{init}}, $name."(".substr($original_name, -1, 1).")";
    push @{$def->{cons}}, $name.".configGpio(".$rx_pin.", ".$tx_pin.");";

}

=head1 common_part

   common_part("DEVICE_A", "DEVICE_B")

Determine the longest common part to all items in the list

This is used to determine the peripheral name when such peripheral is connected
to several IOs for the same function - such as the various pins for a bus
controller, the two pins of a quadrature encoder etc.

=cut
sub common_part
{
    my ($first, $second, @remainder)=@_;
    my $common = String::LCSS::lcss($first, $second);
    $common=~s/^_*([^_].+[^_])_*$/$1/;
    if(@remainder)
    {
	return common_part($common, @remainder);
    }
    else
    {
	return $common;
    }
}

sub process_adc
{
    my ($def, $periph,$issued_adcs) = @_;

    foreach my $analog_key (sort keys %$periph)
    {
	my $analog = $periph->{$analog_key};
	my $obj_name = "ADC_".$analog->{PINS};
	next if $issued_adcs->{$obj_name};
	$issued_adcs->{$obj_name}=1;
	process_gpio_decl($def, $analog, $obj_name);
	push @{$def->{cons}}, $obj_name.".setDirection(Platform::Gpio::ANALOG);";
    }
}

sub process_gpio_decl
{
    my ($def, $portspec, $obj_name)=@_;
    push @{$def->{decl}}, "Platform::Gpio $obj_name;";
    push @{$def->{init}}, $obj_name."(".gpio_name($portspec->{PINS}||$portspec->{PINs}).")";
}

sub gpio_name
{
    my ($pinspec)=@_;
    if(ref($pinspec) eq "HASH")
    {
	$pinspec=$pinspec->{PINS};
    }
    return "Gpio".gpio_port($pinspec) ."[".gpio_pin($pinspec)."]";
}

sub gpio_port
{
    return $1 if($_[0]=~/P([A-Z])(\d+)/);
    return "";
}
sub gpio_pin
{
    return $2 if($_[0]=~/P([A-Z])(\d+)/);
    return "";
}

sub code_gen
{
my ($baseclass, $class, $defs)=@_;
my $headerguard = uc($class)."_H";
my $declarations = join("\n", map{"        ".$_} @{$defs->{decl}});
my $init = join(",\n", map{"         ".$_} @{$defs->{init}});
my $constructor=join("\n", map {"    ".$_} @{$defs->{cons}});
my $includes=join("\n", uniq @{$defs->{includes}});

my $header = <<"EOF"
#ifndef $headerguard
#define $headerguard

#include <STM32F4.hpp>
$includes

class $class: public $baseclass
{
    public:
	$class();
$declarations
};

#endif /* $headerguard */
EOF
;
my $class_file = <<"EOF"

#include "$class.hpp"

$class::$class(): $baseclass(),
$init
{
$constructor
}

EOF
;

return ($header, $class_file);
}

sub consume_header
{
    my ($f)=@_;
    my %retval;
    my $inBlock=1;
    while(defined(my $line=<$f>) and $inBlock)
    {
	chomp $line;
	if($line=~/^([^\t]+)\t(.*)$/)
	{    $retval{$1}=$2; }
	else
	{    $inBlock=0; }
    }
    return \%retval;
}

sub consume_peripherals
{
    return consume_block(@_);
}
sub consume_pins
{
    return consume_block(@_);
}

sub consume_block
{
    my ($f)=@_;
    my @retval;
    my $inBlock=1;
    while(defined(my $line=<$f>) and $inBlock)
    {
	chomp $line;
	if($line=~/^([^\t]+)\t([^\t]+)\t([^\t]+)\t(.*)$/)
	{
	    push @retval, [$1, $2, $3, $4];
	}
	elsif(@retval)
	{    $inBlock=0; }
    }
    my $header = shift @retval;
    my @v;
    foreach my $l (@retval)
    {
	my %map = map {$header->[$_]=>$l->[$_]} (0..3);
	push @v, \%map;
    }
    return \@v;
}

sub parse_cube_spec
{
    my ($fname)=@_;
    my $f = new IO::File($fname, "r") or die("Cannot open $fname for reading");
    my $retval = {};
    while(my $line = <$f>)
    {
	chomp $line;
	next if $line =~/^#/;
	if($line=~/^([^=]+)=(.*)$/)
	{
	    my $path = $1;
	    my $value = $2;
	    my @path = split /\./, $path;
	    my $tree_pos = $retval;
	    my $last_item = pop @path;
	    foreach my $element (@path)
	    {
		$tree_pos->{$element}={} unless exists $tree_pos->{$element};
		$tree_pos = $tree_pos->{$element};
	    }
	    $tree_pos->{$last_item} = $value;
	}
    }
    return $retval;
}



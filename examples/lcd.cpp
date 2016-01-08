#include <CherryPickerBoard.h>
#include <tr1/functional>
#include <Log.h>
#include <Lcd.h>

int main() {
    CherryPickerBoard b;
#if 0
	Lcd lcd(b.Lcd_RS, b.Lcd_E, b.Lcd_DB7, b.Lcd_DB6, b.Lcd_DB5, b.Lcd_DB4);

	lcd(0) << "aaaabb" << endl;
	lcd(1) << "aaaabb" << endl;
#endif
	while(1)
		b.time.msleep(1000);
}

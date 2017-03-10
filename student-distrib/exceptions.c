#include "exceptions.h"

void exception_0() { exception_handler(0);}
void exception_1() { exception_handler(1);}
void exception_2() { exception_handler(2);}
void exception_3() { exception_handler(3);}
void exception_4() { exception_handler(4);}
void exception_5() { exception_handler(5);}
void exception_6() { exception_handler(6);}
void exception_7() { exception_handler(7);}
void exception_8() { exception_handler(8);}
void exception_9() { exception_handler(9);}
void exception_10() { exception_handler(10);}
void exception_11() { exception_handler(11);}
void exception_12() { exception_handler(12);}
void exception_13() { exception_handler(13);}
void exception_14() { exception_handler(14);}
void exception_15() { exception_handler(15);}
void exception_16() { exception_handler(16);}
void exception_17() { exception_handler(17);}
void exception_18() { exception_handler(18);}
void exception_19() { exception_handler(19);}
void exception_20() { exception_handler(20);}
void exception_21() { exception_handler(21);}
void exception_22() { exception_handler(22);}
void exception_23() { exception_handler(23);}
void exception_24() { exception_handler(24);}
void exception_25() { exception_handler(25);}
void exception_26() { exception_handler(26);}
void exception_27() { exception_handler(27);}
void exception_28() { exception_handler(28);}
void exception_29() { exception_handler(29);}
void exception_30() { exception_handler(30);}
void exception_31() { exception_handler(31);}

void exception_handler(int i) {
    clear();
    printf("Error %d\n", i);
    show_blue_screen();
    while(1);
}

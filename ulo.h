#ifndef ULO
#define ULO
#include <QObject>

#define MAX_ROWS 0x7FFC
#define MIN_IDX  -1

/** Logical cell  */
#define LY_1   1
#define LY_2   2
#define LY_3   3

/** Elements */
#define OR      0
#define OR_STR "0"

#define OR_N    1
#define OR_N_STR "1"

#define AND     2
#define AND_STR "2"

#define AND_N   3
#define AND_N_STR "3"

#define UNI     4
#define UNI_STR "4"

#define O_UT    5
#define O_UT_STR "5"

#define INP     6
#define INP_STR "6"

#define NOP 7
#define NOP_STR "7"

//????Требует проверки????//
#define UNI_TG  7
#define UNI_TG_STR "7"

#define TMR     8
#define TMR_STR "8"

#define UNI_2_3 9
#define UNI_2_3_STR "9"

#define UNI_F9  10
#define UNI_F9_STR "10"

#define UNI_99  11
#define UNI_99_STR "11"

#define UNI_41  12
#define UNI_41_STR "12"

#define BVR     13
#define BVR_STR "13"

#define IN_S 1
#define IN_S_STR "1"

#define IN_R 2
#define IN_R_STR "2"

#define IN_X 3
#define IN_X_STR "3"
//?????????????????????//

class Ulo : public QObject{
    //Q_OBJECT

public:
    Q_ENUMS(Orientation)
    enum Orientation {
        /** вВерх */
        North,
        /** вПраво */
        East,
        /** вНиз */
        South,
        /** вЛево */
        West
    };

    Q_ENUMS(ConductorSegmentType)
    enum ConductorSegmentType {
        /** Horizontal segment */
        Horizontal = 1,
        /** Vertical segment */
        Vertical = 2,
        /** Наклонный segment */
        Both = 3
    };

    Q_ENUMS (Direction)
    enum Direction{LEFT, RIGHT, UP, DOWN, LINEERR=-1};


};

#endif // ULO


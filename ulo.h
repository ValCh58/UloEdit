#ifndef ULO
#define ULO
#include <QObject>



#define OR      0
#define OR_N    1
#define AND     2
#define AND_N   3
#define UNI     4
#define O_UT    5
#define INP     6
#define UNI_TG  7
#define TMR     8
#define UNI_2_3 9
#define UNI_F9  10
#define UNI_99  11
#define UNI_41  12
#define BVR     13

#define IN_S 1
#define IN_R 2
#define IN_X 3

class Ulo : public QObject{
    //Q_OBJECT

public:
    Q_ENUMS(Orientation)
    enum Orientation {North, East, South, West};

    Q_ENUMS(ConductorSegmentType)
    enum ConductorSegmentType {
        Horizontal = 1, // Horizontal segment
        Vertical = 2,   // Vertical segment
        Both = 3        // Наклонный segment
    };

    Q_ENUMS (Direction)
    enum Direction{LEFT, RIGHT, UP, DOWN, LINEERR=-1};


};

#endif // ULO


//================================================================================
// Style.h
//================================================================================

#ifndef Style_H
#define Style_H

#include <QProxyStyle>
#include <QPainter>

//================================================================================
// class Style : public QCommonStyle
//================================================================================
//class Style : public QCommonStyle {
class Style : public QProxyStyle {
  Q_OBJECT

  public:
    explicit Style();

    void drawControl(ControlElement element, const QStyleOption *opt, QPainter *p, const QWidget *w) const;
    //void drawItemText(QPainter *painter, const QRect &rect, int flags, const QPalette &pal, bool enabled, const QString &text, QPalette::ColorRole textRole) const;
    void drawPrimitive(PrimitiveElement pe, const QStyleOption *opt, QPainter *p, const QWidget *w) const;

//  private:
//    static QPainterPath roundRectPath(const QRect &rect);
};  // class Style : public QCommonStyle

#endif // Style_H

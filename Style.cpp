//================================================================================
// Style.cpp
//================================================================================

#include <QApplication>
#include <QStyleOptionViewItemV4>
#include <QProxyStyle>
#include <QLinearGradient>
#include <QPalette>
#include <QStyleFactory>

#include "Style.h"


Style::Style() : QProxyStyle(QStyleFactory::create("windows")) {
}


void Style::drawControl(QStyle::ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const {

  switch(element) {
    case CE_ShapedFrame:  // Рамка, вокруг таблицы
      {
        // Верхняя и нижняя границы рамки будут белыми, а вертикальные границы будут нарисованы градиентом:
        // они будут белыми по краям и светло-серыми в середине
        QLinearGradient gradient(0, 0, 0, option->rect.height());
        gradient.setColorAt(0, QColor(255, 255, 255));
        gradient.setColorAt(0.5, QColor(225, 225, 225));
        gradient.setColorAt(1, QColor(255, 255, 255));

        QPen pen;

        pen.setBrush(gradient);
        painter->setPen(pen);

        // Рисуем градиентную рамку
        painter->drawRect(0, 0, option->rect.width() - 1, option->rect.height() - 1);
      }

      break;
    case CE_HeaderEmptyArea:  // Область пустого заголовка
      // Закрасим ее однотонным серым цветом
      painter->fillRect(option->rect, QBrush(QColor(233, 233, 233)));

      break;
    case CE_HeaderSection:  // Область секции (колонки) заголовка
      {
        // State_Sunken показывает, что секция нажата,
        //а флаг State_On показывает, что выбрана ячейка, принадлежащая столбцу (или строке) этой секции
        if(option->state & State_Sunken || option->state & State_On) {  // Активная секция
          // Закрашиваем выделенную секцию серым цветом
          painter->fillRect(option->rect, QBrush(QColor(196, 196, 196)));

          painter->save();

          painter->setPen(QPen(QColor(170, 170, 170)));

          // Рисуем рамку вокруг секции
          painter->drawRect(option->rect.x(), option->rect.y(), option->rect.width() - 1, option->rect.height() - 1);

          painter->restore();
        } else {  // Неактивная секция
          // Создаем градиент для фона

          QLinearGradient gradient(0, 0, 0, option->rect.height());
          gradient.setSpread(QGradient::PadSpread);
          gradient.setColorAt(0, QColor(255, 255, 255));
          gradient.setColorAt(1, QColor(220, 220, 220));

          // Рисуем фон

          painter->fillRect(option->rect, QBrush(gradient));

          gradient.setColorAt(0, QColor(230, 230, 230));
          gradient.setColorAt(0.5, QColor(175, 175, 175));
          gradient.setColorAt(1, QColor(230, 230, 230));

          QPen pen;

          pen.setStyle(Qt::SolidLine);
          pen.setBrush(gradient);
          painter->setPen(pen);

          // Рисуем границы секций
          painter->drawLine(
              option->rect.width()  + option->rect.x() - 1,
              option->rect.y() + 3,
              option->rect.width()  + option->rect.x() - 1,
              option->rect.height() + option->rect.y() - 3
          );
        }  // if(option->state & State_Sunken || option->state & State_On)
      }

      break;
    default:
      QProxyStyle::drawControl(element, option, painter, widget);

      break;
  }
}


void Style::drawPrimitive(QStyle::PrimitiveElement pe, const QStyleOption *option, QPainter *painter, const QWidget *widget) const {
  switch(pe) {
    case PE_PanelItemViewRow:  // Область выделенного ряда таблицы
    case PE_PanelItemViewItem:  // Область выделенной ячейки таблицы
      if(const QStyleOptionViewItemV4 *opt = qstyleoption_cast<const QStyleOptionViewItemV4 *>(option)) {
        // Цвет особо закрашенного фона ячейки (в остальных случаях - черный цвет)
        QColor color = opt->backgroundBrush.color();

        // Определяем цветовую гамму
        int red = color.red();
        int green = color.green();
        int blue = color.blue();

        if(option->state & QStyle::State_Selected) {  // либо QStyle::State_HasFocus
          // Активные ячейки

          if(red + green + blue == 0) {  // Цвет фона черный (что верно и для любого базового либо альтернативного цветов ячеек)
            // Здесь нет особо закрашенных ячеек, т.е. это обычные либо альернативыне невыделенные ячейки

            // Определяем факт использования альтернативной расскраски ячеек
            if(opt->features & QStyleOptionViewItem::Alternate) {  // Альтернативно закрашиваемый ряд
              // Определяем цвет альтернативной расскраски
              color = opt->palette.alternateBase().color();
            } else {  // Обычно закрашиваемый ряд
              // Определяем цвет обычной расскраски
              color = opt->palette.background().color();
            }

            // Определяем цветовую гамму
            red = color.red();
            green = color.green();
            blue = color.blue();

            // Получаем размер области, на которой будет отображаться текст
            //QRect textRect = subElementRect(SE_ItemViewItemText, opt, widget);

            // Выводим парметры цветов в ячейки
            //painter->drawText(textRect, QString("(%1,%2,%3)").arg(red).arg(green).arg(blue));
          }  // if(red + green + blue == 0)

          // Затемнение данного цвета с выбранным парметром затемнения по умолчанию
          painter->fillRect(opt->rect, QColor(red, green, blue).dark(150));
        } else {  // Неактивные ячейки (нет фокуса на таблице)
          QProxyStyle::drawPrimitive(pe, opt, painter, widget);
        }  // if(option->state & QStyle::State_Selected)
      } else {  // opt = NULL
        QProxyStyle::drawPrimitive(pe, opt, painter, widget);
      }  // if(const QStyleOptionViewItemV4 *opt = qstyleoption_cast<const QStyleOptionViewItemV4 *>(option))

      break;
    default:
      QProxyStyle::drawPrimitive(pe, option, painter, widget);

      break;
  }
}

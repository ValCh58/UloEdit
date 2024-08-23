/*
	Copyright 2006-2015 The QElectroTech Team
	This file is part of QElectroTech.
	
	QElectroTech is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option) any later version.
	
	QElectroTech is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with QElectroTech.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <QPainter>

#include "bordertitleblock.h"
#include "math.h"
#include "schemealgo.h"

#define MIN_COLUMN_COUNT 3
#define MIN_ROW_COUNT 3
#define MIN_COLUMN_WIDTH 5.0
#define MIN_ROW_HEIGHT 5.0


BorderTitleBlock::BorderTitleBlock(QObject *parent) : QObject(parent)
{
    display_titleblock_ = true;
	display_border_ = true;
    displayColumns(true);
    displayRows(true);
    setColumnsCount(18);
    setColumnsWidth(70);
    setColumnsHeaderHeight(20);
    setRowsCount(40);
    setRowsHeight(60);
    setRowsHeaderWidth(20);
    updateRectangles();
}


BorderTitleBlock::~BorderTitleBlock() {
}


/**
 * @brief BorderTitleBlock::borderAndTitleBlockRect
 * @return the bounding rectangle of diagram and titleblock.
 * It's like unite outsideBorderRect and titleBlockRect.
 * The rect is in scene coordinate
 */
QRectF BorderTitleBlock::borderAndTitleBlockRect() const {
    return diagram_rect_ ;
}

/**
 * @brief BorderTitleBlock::columnsRect
 * @return The columns rect in scene coordinate.
 * If column is not displayed, return a null QRectF
 */
QRectF BorderTitleBlock::columnsRect() const
{
	if (!display_columns_) return QRectF();
    return QRectF (SchemeAlgo::margin, SchemeAlgo::margin, (columns_count_*columns_width_)
                   + rows_header_width_, columns_header_height_);
}

/**
 * @brief BorderTitleBlock::rowsRect
 * @return The rows rect in scene coordinate.
 * If row is not displayed, return a null QRectF
 */
QRectF BorderTitleBlock::rowsRect() const
{
	if (!display_rows_) return QRectF();
    return QRectF (SchemeAlgo::margin, SchemeAlgo::margin, rows_header_width_,
                   (rows_count_*rows_height_) + columns_header_height_);
}

/**
 * @brief BorderTitleBlock::outsideBorderRect
 * @return The rect of outside border (diagram with columns and rows)
 * The rect is in scene coordinate
 */
QRectF BorderTitleBlock::outsideBorderRect() const
{
    return QRectF (SchemeAlgo::margin, SchemeAlgo::margin,
				  (columns_width_*columns_count_) + rows_header_width_,
				  (rows_height_*rows_count_) + columns_header_height_);
}

/**
 * @brief BorderTitleBlock::insideBorderRect
 * @return The rect of the inside border, in other word, the drawing area.
 * This method take care about if rows or columns are displayed or not.
 * The rect is in scene coordinate
 */
QRectF BorderTitleBlock::insideBorderRect() const
{
    qreal left = SchemeAlgo::margin;
    qreal top  = SchemeAlgo::margin;
	qreal width  = columns_width_*columns_count_;
	qreal height = rows_height_*rows_count_;

	display_rows_ ? left += rows_header_width_ : width += rows_header_width_;
	display_columns_ ? top += columns_header_height_ : height += columns_header_height_;

	return QRectF (left, top, width, height);
}



/**
	@param di true pour afficher le cartouche, false sinon
*/
void BorderTitleBlock::displayTitleBlock(bool di) {
	bool change = (di != display_titleblock_);
	display_titleblock_ = di;
	if (change) emit(displayChanged());
}

/**
	@param dc true pour afficher les entetes des colonnes, false sinon
*/
void BorderTitleBlock::displayColumns(bool dc) {
	bool change = (dc != display_columns_);
	display_columns_ = dc;
	if (change) emit(displayChanged());
}

/**
	@param dr true pour afficher les entetes des lignes, false sinon
*/
void BorderTitleBlock::displayRows(bool dr) {
	bool change = (dr != display_rows_);
	display_rows_ = dr;
	if (change) emit(displayChanged());
}

/**
	@param db true pour afficher la bordure du schema, false sinon
	Note : si l'affichage de la bordure est ainsi desactivee, les lignes et
	colonnes ne seront pas dessinees.
*/
void BorderTitleBlock::displayBorder(bool db) {
	bool change = (db != display_border_);
	display_border_  = db;
	if (change) emit(displayChanged());
}

/**
 * @brief BorderTitleBlock::updateRectangles
 * This method update the diagram rect according to the value of rows and columns (number and size)
 */
void BorderTitleBlock::updateRectangles()
{
    QRectF previous_diagram = diagram_rect_;
    diagram_rect_ = QRectF(SchemeAlgo::margin, SchemeAlgo::margin, diagramWidth(), diagramHeight());
    if (diagram_rect_ != previous_diagram) emit(borderChanged(previous_diagram, diagram_rect_));
}

/**
 * @brief BorderTitleBlock::draw
 * Draw the border and the titleblock.
 * @param painter, QPainter to use for draw this.
 */
void BorderTitleBlock::draw(QPainter *painter)
{// ----------------------------------------------------------------------------------------------
    //Set the QPainter
	painter -> save();
	QPen pen(Qt::black);
	pen.setCosmetic(true);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
	
    //Draw the borer
    if (display_border_)
        painter->drawRect(diagram_rect_);
	    	
    //Draw the empty case at the top left of diagram when there is header
    if (display_border_ && (display_columns_ || display_rows_))
	{
		QRectF first_rectangle(
			diagram_rect_.topLeft().x(),
			diagram_rect_.topLeft().y(),
			rows_header_width_,
			columns_header_height_
		);
        painter->drawRect(first_rectangle);
	}
	
    //Draw the nums of columns
    if (display_border_ && display_columns_) {
		for (int i = 1 ; i <= columns_count_ ; ++ i) {
			QRectF numbered_rectangle = QRectF(
				diagram_rect_.topLeft().x() + (rows_header_width_ + ((i - 1) * columns_width_)),
				diagram_rect_.topLeft().y(),
				columns_width_,
				columns_header_height_
			);
            painter->drawRect(numbered_rectangle);
            painter->drawText(numbered_rectangle, Qt::AlignVCenter | Qt::AlignCenter, QString("%1").arg(i));
		}
	}
	
    //Draw the nums of rows
	if (display_border_ && display_rows_) {
		QString row_string("A");
		for (int i = 1 ; i <= rows_count_ ; ++ i) {
			QRectF lettered_rectangle = QRectF(
				diagram_rect_.topLeft().x(),
				diagram_rect_.topLeft().y() + (columns_header_height_ + ((i - 1) * rows_height_)),
				rows_header_width_,
				rows_height_
			);
            painter->drawRect(lettered_rectangle);
            painter->drawText(lettered_rectangle, Qt::AlignVCenter | Qt::AlignCenter, row_string);
			row_string = incrementLetters(row_string);
		}
    }
	
    painter->restore();
}// -----------------------------------------------------------------------------------------------------------

/**
	Permet de changer le nombre de colonnes.
	Si ce nombre de colonnes est inferieur au minimum requis, c'est ce minimum
	qui est utilise.
	@param nb_c nouveau nombre de colonnes
	@see minNbColumns()
*/
void BorderTitleBlock::setColumnsCount(int nb_c) {
	if (nb_c == columnsCount()) return;
	columns_count_ = qMax(MIN_COLUMN_COUNT , nb_c);
	updateRectangles();
}

/**
	Change la largeur des colonnes.
	Si la largeur indiquee est inferieure au minimum requis, c'est ce minimum
	qui est utilise.
	@param new_cw nouvelle largeur des colonnes
	@see minColumnsWidth()
*/
void BorderTitleBlock::setColumnsWidth(const qreal &new_cw) {
	if (new_cw == columnsWidth()) return;
	columns_width_ = qMax(MIN_COLUMN_WIDTH , new_cw);
	updateRectangles();
}

/**
	Change la hauteur des en-tetes contenant les numeros de colonnes. Celle-ci
	doit rester comprise entre 5 et 50 px.
	@param new_chh nouvelle hauteur des en-tetes de colonnes
*/
void BorderTitleBlock::setColumnsHeaderHeight(const qreal &new_chh) {
	columns_header_height_ = qBound(qreal(5.0), new_chh, qreal(50.0));
	updateRectangles();
}

/**
	Permet de changer le nombre de lignes.
	Si ce nombre de lignes est inferieur au minimum requis, cette fonction ne
	fait rien
	@param nb_r nouveau nombre de lignes
	@see minNbRows()
*/
void BorderTitleBlock::setRowsCount(int nb_r) {
	if (nb_r == rowsCount()) return;
	rows_count_ = qMax(MIN_ROW_COUNT, nb_r);
	updateRectangles();
}

/**
	Change la hauteur des lignes.
	Si la hauteur indiquee est inferieure au minimum requis, c'est ce minimum
	qui est utilise.
	@param new_rh nouvelle hauteur des lignes
	@see minRowsHeight()
*/
void BorderTitleBlock::setRowsHeight(const qreal &new_rh) {
	if (new_rh == rowsHeight()) return;
	rows_height_ = qMax(MIN_ROW_HEIGHT, new_rh);
	updateRectangles();
}

/**
	Change la largeur des en-tetes contenant les numeros de lignes. Celle-ci
	doit rester comprise entre 5 et 50 px.
	@param new_rhw nouvelle largeur des en-tetes des lignes
*/
void BorderTitleBlock::setRowsHeaderWidth(const qreal &new_rhw) {
	rows_header_width_ = qBound(qreal(5.0), new_rhw, qreal(50.0));
	updateRectangles();
}

/**
	Cette methode essaye de se rapprocher le plus possible de la hauteur donnee
	en parametre en modifiant le nombre de lignes en cours.
*/
void BorderTitleBlock::setDiagramHeight(const qreal &height) {
	// taille des lignes a utiliser = rows_height
	setRowsCount(qRound(ceil(height / rows_height_)));
}


QString BorderTitleBlock::incrementLetters(const QString &string) {
	if (string.isEmpty()) {
		return("A");
	} else {
		// separe les digits precedents du dernier digit
		QString first_digits(string.left(string.count() - 1));
		QChar last_digit(string.at(string.count() - 1));
		if (last_digit != 'Z') {
			// incremente le dernier digit
			last_digit = last_digit.toLatin1() + 1;
			return(first_digits + QString(last_digit));
		} else {
			return(incrementLetters(first_digits) + "A");
		}
	}
}


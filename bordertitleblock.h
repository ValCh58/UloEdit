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
#ifndef BORDERTITLEBLOCK_H
#define BORDERTITLEBLOCK_H
#include <QObject>
#include <QRectF>
#include <QDate>
class QPainter;
//class DiagramPosition;


/**
	This class represents the border and the titleblock which frame a
	particular electric diagram.
*/
class BorderTitleBlock : public QObject
{
		Q_OBJECT
	
	public:
		BorderTitleBlock(QObject * = 0);
		virtual ~BorderTitleBlock();
	
	private:
		BorderTitleBlock(const BorderTitleBlock &);
	
		//METHODS
	public:	
		void draw(QPainter *painter);
		void drawDxf(int, int, bool, QString &, int);
	
		//METHODS TO GET DIMENSION
		//COLUMNS
			/// @return the number of columns
		int   columnsCount() const { return(columns_count_); }
			/// @return the columns width, in pixels
		qreal columnsWidth() const { return(columns_width_); }
			/// @return the total width of all columns, headers excluded
		qreal columnsTotalWidth() const { return(columns_count_ * columns_width_); }
			/// @return the column headers height, in pixels
		qreal columnsHeaderHeight() const { return(columns_header_height_); }
	
		//ROWS
			/// @return the number of rows
		int rowsCount() const { return(rows_count_); }
			/// @return the rows height, in pixels
		qreal rowsHeight() const { return(rows_height_); }
			/// @return the total height of all rows, headers excluded
		qreal rowsTotalHeight() const { return(rows_count_ * rows_height_); }
			/// @return la rows header width, in pixels
		qreal rowsHeaderWidth() const { return(rows_header_width_); }
	
		// border - title block = diagram
			/// @return the diagram width, i.e. the width of the border without title block
		qreal diagramWidth() const { return(columnsTotalWidth() + rowsHeaderWidth()); }
			/// @return the diagram height, i.e. the height of the border without title block
		qreal diagramHeight() const { return(rowsTotalHeight() + columnsHeaderHeight()); }




	public:
		QRectF borderAndTitleBlockRect () const;
		QRectF columnsRect () const;
		QRectF rowsRect () const;
		QRectF outsideBorderRect() const;
		QRectF insideBorderRect() const;
	

	// methods to get display options
	/// @return true si le cartouche est affiche, false sinon
	bool titleBlockIsDisplayed() const { return(display_titleblock_); }
	/// @return true si les entetes des colonnes sont affiches, false sinon
	bool columnsAreDisplayed() const { return(display_columns_); }
	/// @return true si les entetes des lignes sont affiches, false sinon
	bool rowsAreDisplayed() const { return(display_rows_); }
	/// @return true si la bordure est affichee, false sinon
	bool borderIsDisplayed() const { return(display_border_); }
	
	// methods to set dimensions
	void setColumnsCount(int);
	void setRowsCount(int);
	void setColumnsWidth(const qreal &);
	void setRowsHeight(const qreal &);
	void setColumnsHeaderHeight(const qreal &);
	void setRowsHeaderWidth(const qreal &);
	void setDiagramHeight(const qreal &);
	

	public slots:

	// methods to set display options
	void displayTitleBlock(bool);
	void displayColumns(bool);
	void displayRows(bool);
	void displayBorder(bool);
	
	private:
	void updateRectangles();
    //void updateDiagramContextForTitleBlock(const DiagramContext & = DiagramContext());
	QString incrementLetters(const QString &);
	
	signals:
	/**
		Signal emitted after the border has changed
		@param old_border Former border
		@param new_border New border
	*/
	void borderChanged(QRectF old_border, QRectF new_border);
	/**
		Signal emitted after display options have changed
	*/
	void displayChanged();
	
	/**
		Signal emitted after the title has changed
	*/
	void diagramTitleChanged(const QString &);
	
	/**
		Signal emitted when the title block requires its data to be updated in order
		to generate the folio field.
	*/
	void needFolioData();
	
	/**
		Signal emitted when this object needs to set a specific title block
		template. This object cannot handle the job since it does not know of
		its parent project.
	*/
	void needTitleBlockTemplate(const QString &);
	
		// attributes
	private:

        //DiagramContext additional_fields_;
		Qt::Edge m_edge;
	
		// border dimensions (rows and columns)
		// columns: number and dimensions
		int columns_count_;
		qreal columns_width_;
		qreal columns_header_height_;
	
		// rows: number and dimensions
		int rows_count_;
		qreal rows_height_;
		qreal rows_header_width_;
	
		// title block dimensions
		qreal titleblock_height_;
	
		// rectangles used for drawing operations
		QRectF diagram_rect_;
	
		// display options
		bool display_titleblock_;
		bool display_columns_;
		bool display_rows_;
		bool display_border_;
        //TitleBlockTemplateRenderer *titleblock_template_renderer_;
};
#endif

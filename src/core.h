/***************************************************************************
**                                                                        **
**  QCustomPlot, an easy to use, modern plotting widget for Qt            **
**  Copyright (C) 2011, 2012, 2013, 2014 Emanuel Eichhammer               **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Emanuel Eichhammer                                   **
**  Website/Contact: http://www.qcustomplot.com/                          **
**             Date: 07.04.14                                             **
**          Version: 1.2.1                                                **
****************************************************************************/

#ifndef QCP_CORE_H
#define QCP_CORE_H

#include "global.h"
#include "range.h"
#include "axis.h"

class QCPPainter;
class QCPLayer;
class QCPAbstractPlottable;
class QCPAbstractItem;
class QCPGraph;
class QCPPlotTitle;
class QCPLegend;
class QCPAbstractLegendItem;

class QCP_LIB_DECL QCustomPlot : public QWidget
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(QRect viewport READ viewport WRITE setViewport)
  Q_PROPERTY(QPixmap background READ background WRITE setBackground)
  Q_PROPERTY(bool backgroundScaled READ backgroundScaled WRITE setBackgroundScaled)
  Q_PROPERTY(Qt::AspectRatioMode backgroundScaledMode READ backgroundScaledMode WRITE setBackgroundScaledMode)
  Q_PROPERTY(QCPLayoutGrid* plotLayout READ plotLayout)
  Q_PROPERTY(bool autoAddPlottableToLegend READ autoAddPlottableToLegend WRITE setAutoAddPlottableToLegend)
  Q_PROPERTY(int selectionTolerance READ selectionTolerance WRITE setSelectionTolerance)
  Q_PROPERTY(bool noAntialiasingOnDrag READ noAntialiasingOnDrag WRITE setNoAntialiasingOnDrag)
  Q_PROPERTY(Qt::KeyboardModifier multiSelectModifier READ multiSelectModifier WRITE setMultiSelectModifier)
  /// \endcond
public:
  /*!
    Defines how a layer should be inserted relative to an other layer.

    \see addLayer, moveLayer
  */
  enum LayerInsertMode { limBelow  ///< Layer is inserted below other layer
                         ,limAbove ///< Layer is inserted above other layer
                       };
  Q_ENUMS(LayerInsertMode)
  
  /*!
    Defines with what timing the QCustomPlot surface is refreshed after a replot.

    \see replot
  */
  enum RefreshPriority { rpImmediate ///< The QCustomPlot surface is immediately refreshed, by calling QWidget::repaint() after the replot
                         ,rpQueued   ///< Queues the refresh such that it is performed at a slightly delayed point in time after the replot, by calling QWidget::update() after the replot
                         ,rpHint     ///< Whether to use immediate repaint or queued update depends on whether the plotting hint \ref QCP::phForceRepaint is set, see \ref setPlottingHints.
                       };
  
  explicit QCustomPlot(QWidget *parent = 0);
  virtual ~QCustomPlot();
  
  // getters:
  QRect viewport() const { return mViewport; }
  QPixmap background() const { return mBackgroundPixmap; }
  bool backgroundScaled() const { return mBackgroundScaled; }
  Qt::AspectRatioMode backgroundScaledMode() const { return mBackgroundScaledMode; }
  QCPLayoutGrid *plotLayout() const { return mPlotLayout; }
  QCP::AntialiasedElements antialiasedElements() const { return mAntialiasedElements; }
  QCP::AntialiasedElements notAntialiasedElements() const { return mNotAntialiasedElements; }
  bool autoAddPlottableToLegend() const { return mAutoAddPlottableToLegend; }
  const QCP::Interactions interactions() const { return mInteractions; }
  int selectionTolerance() const { return mSelectionTolerance; }
  bool noAntialiasingOnDrag() const { return mNoAntialiasingOnDrag; }
  QCP::PlottingHints plottingHints() const { return mPlottingHints; }
  Qt::KeyboardModifier multiSelectModifier() const { return mMultiSelectModifier; }

  // setters:
  void setViewport(const QRect &rect);
  void setBackground(const QPixmap &pm);
  void setBackground(const QPixmap &pm, bool scaled, Qt::AspectRatioMode mode=Qt::KeepAspectRatioByExpanding);
  void setBackground(const QBrush &brush);
  void setBackgroundScaled(bool scaled);
  void setBackgroundScaledMode(Qt::AspectRatioMode mode);
  void setAntialiasedElements(const QCP::AntialiasedElements &antialiasedElements);
  void setAntialiasedElement(QCP::AntialiasedElement antialiasedElement, bool enabled=true);
  void setNotAntialiasedElements(const QCP::AntialiasedElements &notAntialiasedElements);
  void setNotAntialiasedElement(QCP::AntialiasedElement notAntialiasedElement, bool enabled=true);
  void setAutoAddPlottableToLegend(bool on);
  void setInteractions(const QCP::Interactions &interactions);
  void setInteraction(const QCP::Interaction &interaction, bool enabled=true);
  void setSelectionTolerance(int pixels);
  void setNoAntialiasingOnDrag(bool enabled);
  void setPlottingHints(const QCP::PlottingHints &hints);
  void setPlottingHint(QCP::PlottingHint hint, bool enabled=true);
  void setMultiSelectModifier(Qt::KeyboardModifier modifier);
  
  // non-property methods:
  // plottable interface:
  QCPAbstractPlottable *plottable(int index);
  QCPAbstractPlottable *plottable();
  bool addPlottable(QCPAbstractPlottable *plottable);
  bool removePlottable(QCPAbstractPlottable *plottable);
  bool removePlottable(int index);
  int clearPlottables();
  int plottableCount() const;
  QList<QCPAbstractPlottable*> selectedPlottables() const;
  QCPAbstractPlottable *plottableAt(const QPointF &pos, bool onlySelectable=false) const;
  bool hasPlottable(QCPAbstractPlottable *plottable) const;
 
  // specialized interface for QCPGraph:
  QCPGraph *graph(int index) const;
  QCPGraph *graph() const;
  QCPGraph *addGraph(QCPAxis *keyAxis=0, QCPAxis *valueAxis=0);
  bool removeGraph(QCPGraph *graph);
  bool removeGraph(int index);
  int clearGraphs();
  int graphCount() const;
  QList<QCPGraph*> selectedGraphs() const;

  // item interface:
  QCPAbstractItem *item(int index) const;
  QCPAbstractItem *item() const;
  bool addItem(QCPAbstractItem* item);
  bool removeItem(QCPAbstractItem *item);
  bool removeItem(int index);
  int clearItems();
  int itemCount() const;
  QList<QCPAbstractItem*> selectedItems() const;
  QCPAbstractItem *itemAt(const QPointF &pos, bool onlySelectable=false) const;
  bool hasItem(QCPAbstractItem *item) const;
  
  // layer interface:
  QCPLayer *layer(const QString &name) const;
  QCPLayer *layer(int index) const;
  QCPLayer *currentLayer() const;
  bool setCurrentLayer(const QString &name);
  bool setCurrentLayer(QCPLayer *layer);
  int layerCount() const;
  bool addLayer(const QString &name, QCPLayer *otherLayer=0, LayerInsertMode insertMode=limAbove);
  bool removeLayer(QCPLayer *layer);
  bool moveLayer(QCPLayer *layer, QCPLayer *otherLayer, LayerInsertMode insertMode=limAbove);
  
  // axis rect/layout interface:
  int axisRectCount() const;
  QCPAxisRect* axisRect(int index=0) const;
  QList<QCPAxisRect*> axisRects() const;
  QCPLayoutElement* layoutElementAt(const QPointF &pos) const;
  Q_SLOT void rescaleAxes(bool onlyVisiblePlottables=false);
  
  QPointF mapToCoord(const QPoint& pos) const;
  QPoint mapFromCoord(const QPointF& pos) const;
  QRectF mapToCoord(const QRect& rect) const;
  QRect mapFromCoord(const QRectF& rect) const;

  QList<QCPAxis*> selectedAxes() const;
  QList<QCPLegend*> selectedLegends() const;
  Q_SLOT void deselectAll();
  
  bool savePdf(const QString &fileName, bool noCosmeticPen=false, int width=0, int height=0, const QString &pdfCreator="", const QString &pdfTitle="");
  bool savePng(const QString &fileName, int width=0, int height=0, double scale=1.0, int quality=-1);
  bool saveJpg(const QString &fileName, int width=0, int height=0, double scale=1.0, int quality=-1);
  bool saveBmp(const QString &fileName, int width=0, int height=0, double scale=1.0);
  bool saveRastered(const QString &fileName, int width, int height, double scale, const char *format, int quality=-1);
  QPixmap toPixmap(int width=0, int height=0, double scale=1.0);
  void toPainter(QCPPainter *painter, int width=0, int height=0);
  Q_SLOT void replot(QCustomPlot::RefreshPriority refreshPriority=QCustomPlot::rpHint);
  
  QCPAxis *xAxis, *yAxis, *xAxis2, *yAxis2;
  QCPLegend *legend;
  
signals:
  void mouseDoubleClick(QMouseEvent *event);
  void mousePress(QMouseEvent *event);
  void mouseMove(QMouseEvent *event);
  void mouseRelease(QMouseEvent *event);
  void mouseWheel(QWheelEvent *event);
  
  void plottableClick(QCPAbstractPlottable *plottable, QMouseEvent *event);
  void plottableDoubleClick(QCPAbstractPlottable *plottable, QMouseEvent *event);
  void itemClick(QCPAbstractItem *item, QMouseEvent *event);
  void itemDoubleClick(QCPAbstractItem *item, QMouseEvent *event);
  void axisClick(QCPAxis *axis, QCPAxis::SelectablePart part, QMouseEvent *event);
  void axisDoubleClick(QCPAxis *axis, QCPAxis::SelectablePart part, QMouseEvent *event);
  void legendClick(QCPLegend *legend, QCPAbstractLegendItem *item, QMouseEvent *event);
  void legendDoubleClick(QCPLegend *legend,  QCPAbstractLegendItem *item, QMouseEvent *event);
  void titleClick(QMouseEvent *event, QCPPlotTitle *title);
  void titleDoubleClick(QMouseEvent *event, QCPPlotTitle *title);
  
  void selectionChangedByUser();
  void beforeReplot();
  void afterReplot();
  
protected:
  // property members:
  QRect mViewport;
  QCPLayoutGrid *mPlotLayout;
  bool mAutoAddPlottableToLegend;
  QList<QCPAbstractPlottable*> mPlottables;
  QList<QCPGraph*> mGraphs; // extra list of plottables also in mPlottables that are of type QCPGraph
  QList<QCPAbstractItem*> mItems;
  QList<QCPLayer*> mLayers;
  QCP::AntialiasedElements mAntialiasedElements, mNotAntialiasedElements;
  QCP::Interactions mInteractions;
  int mSelectionTolerance;
  bool mNoAntialiasingOnDrag;
  QBrush mBackgroundBrush;
  QPixmap mBackgroundPixmap;
  QPixmap mScaledBackgroundPixmap;
  bool mBackgroundScaled;
  Qt::AspectRatioMode mBackgroundScaledMode;
  QCPLayer *mCurrentLayer;
  QCP::PlottingHints mPlottingHints;
  Qt::KeyboardModifier mMultiSelectModifier;
  
  // non-property members:
  QPixmap mPaintBuffer;
  QPoint mMousePressPos;
  QPointer<QCPLayoutElement> mMouseEventElement;
  bool mReplotting;
  
  // reimplemented virtual methods:
  virtual QSize minimumSizeHint() const;
  virtual QSize sizeHint() const;
  virtual void paintEvent(QPaintEvent *event);
  virtual void resizeEvent(QResizeEvent *event);
  virtual void mouseDoubleClickEvent(QMouseEvent *event);
  virtual void mousePressEvent(QMouseEvent *event);
  virtual void mouseMoveEvent(QMouseEvent *event);
  virtual void mouseReleaseEvent(QMouseEvent *event);
  virtual void wheelEvent(QWheelEvent *event);
  
  // introduced virtual methods:
  virtual void draw(QCPPainter *painter);
  virtual void axisRemoved(QCPAxis *axis);
  virtual void legendRemoved(QCPLegend *legend);
  
  // non-virtual methods:
  void updateLayerIndices() const;
  QCPLayerable *layerableAt(const QPointF &pos, bool onlySelectable, QVariant *selectionDetails=0) const;
  void drawBackground(QCPPainter *painter);
  
  friend class QCPLegend;
  friend class QCPAxis;
  friend class QCPLayer;
  friend class QCPAxisRect;
};

#endif // QCP_CORE_H

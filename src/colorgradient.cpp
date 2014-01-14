/***************************************************************************
**                                                                        **
**  QCustomPlot, an easy to use, modern plotting widget for Qt            **
**  Copyright (C) 2011, 2012, 2013 Emanuel Eichhammer                     **
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
**             Date: 09.12.13                                             **
**          Version: 1.1.1                                                **
****************************************************************************/

#include "colorgradient.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// QCPColorGradient
////////////////////////////////////////////////////////////////////////////////////////////////////

/*! \class QCPColorGradient

*/

QCPColorGradient::QCPColorGradient(GradientPreset preset) :
  mLevelCount(350),
  mColorInterpolation(ciRGB),
  mPeriodic(false),
  mColorBufferInvalidated(true)
{
  mColorBuffer.fill(qRgb(0, 0, 0), mLevelCount);
  loadPreset(preset);
}

QCPColorGradient::~QCPColorGradient()
{
}

bool QCPColorGradient::operator==(const QCPColorGradient &other) const
{
  return ((other.mLevelCount == this->mLevelCount) &&
          (other.mColorInterpolation == this->mColorInterpolation) &&
          (other.mPeriodic == this->mPeriodic) &&
          (other.mColorStops == this->mColorStops));
}

void QCPColorGradient::setLevelCount(int n)
{
  if (n < 2)
  {
    qDebug() << Q_FUNC_INFO << "n must be greater or equal 2 but was" << n;
    n = 2;
  }
  if (n != mLevelCount)
  {
    mLevelCount = n;
    mColorBufferInvalidated = true;
  }
}

void QCPColorGradient::setColorStops(const QMap<double, QColor> &colorStops)
{
  mColorStops = colorStops;
  mColorBufferInvalidated = true;
}

void QCPColorGradient::setColorStopAt(double position, const QColor &color)
{
  mColorStops.insert(position, color);
  mColorBufferInvalidated = true;
}

void QCPColorGradient::setColorInterpolation(QCPColorGradient::ColorInterpolation interpolation)
{
  if (interpolation != mColorInterpolation)
  {
    mColorInterpolation = interpolation;
    mColorBufferInvalidated = true;
  }
}

void QCPColorGradient::setPeriodic(bool enabled)
{
  mPeriodic = enabled;
}

void QCPColorGradient::colorize(const double *data, const QCPRange &range, QRgb *scanLine, int n, int dataIndexFactor, bool logarithmic)
{
  // If you change something here, make sure to also adapt ::color()
  if (!data)
  {
    qDebug() << Q_FUNC_INFO << "null pointer given as data";
    return;
  }
  if (!scanLine)
  {
    qDebug() << Q_FUNC_INFO << "null pointer given as scanLine";
    return;
  }
  if (mColorBufferInvalidated)
    updateColorBuffer();
  
  if (!logarithmic)
  {
    const double posToIndexFactor = mLevelCount/range.size();
    if (mPeriodic)
    {
      for (int i=0; i<n; ++i)
      {
        int index = (int)((data[dataIndexFactor*i]-range.lower)*posToIndexFactor) % mLevelCount;
        if (index < 0)
          index += mLevelCount;
        scanLine[i] = mColorBuffer.at(index);
      }
    } else
    {
      for (int i=0; i<n; ++i)
      {
        int index = (data[dataIndexFactor*i]-range.lower)*posToIndexFactor;
        if (index < 0)
          index = 0;
        else if (index >= mLevelCount)
          index = mLevelCount-1;
        scanLine[i] = mColorBuffer.at(index);
      }
    }
  } else // logarithmic == true
  {
    if (mPeriodic)
    {
      for (int i=0; i<n; ++i)
      {
        int index = (int)(qLn(data[dataIndexFactor*i]/range.lower)/qLn(range.upper/range.lower)*mLevelCount) % mLevelCount;
        if (index < 0)
          index += mLevelCount;
        scanLine[i] = mColorBuffer.at(index);
      }
    } else
    {
      for (int i=0; i<n; ++i)
      {
        int index = qLn(data[dataIndexFactor*i]/range.lower)/qLn(range.upper/range.lower)*mLevelCount;
        if (index < 0)
          index = 0;
        else if (index >= mLevelCount)
          index = mLevelCount-1;
        scanLine[i] = mColorBuffer.at(index);
      }
    }
  }
}

QRgb QCPColorGradient::color(double position, const QCPRange &range, bool logarithmic)
{
  // If you change something here, make sure to also adapt ::colorize()
  if (mColorBufferInvalidated)
    updateColorBuffer();
  int index = 0;
  if (!logarithmic)
    index = (position-range.lower)*mLevelCount/range.size();
  else
    index = qLn(position/range.lower)/qLn(range.upper/range.lower)*mLevelCount;
  if (mPeriodic)
  {
    index = index % mLevelCount;
    if (index < 0)
      index += mLevelCount;
  } else
  {
    if (index < 0)
      index = 0;
    else if (index >= mLevelCount)
      index = mLevelCount-1;
  }
  return mColorBuffer.at(index);
}

void QCPColorGradient::loadPreset(GradientPreset preset)
{
  clearColorStops();
  switch (preset)
  {
    case gpGrayscale:
      setColorInterpolation(ciRGB);
      setColorStopAt(0, Qt::black);
      setColorStopAt(1, Qt::white);
      break;
    case gpHot:
      setColorInterpolation(ciRGB);
      setColorStopAt(0, QColor(50, 0, 0));
      setColorStopAt(0.2, QColor(180, 10, 0));
      setColorStopAt(0.4, QColor(245, 50, 0));
      setColorStopAt(0.6, QColor(255, 150, 10));
      setColorStopAt(0.8, QColor(255, 255, 50));
      setColorStopAt(1, QColor(255, 255, 255));
      break;
    case gpCold:
      setColorInterpolation(ciRGB);
      setColorStopAt(0, QColor(0, 0, 50));
      setColorStopAt(0.2, QColor(0, 10, 180));
      setColorStopAt(0.4, QColor(0, 50, 245));
      setColorStopAt(0.6, QColor(10, 150, 255));
      setColorStopAt(0.8, QColor(50, 255, 255));
      setColorStopAt(1, QColor(255, 255, 255));
      break;
    case gpNight:
      setColorInterpolation(ciHSV);
      setColorStopAt(0, QColor(10, 20, 30));
      setColorStopAt(1, QColor(250, 255, 250));
      break;
    case gpCandy:
      setColorInterpolation(ciHSV);
      setColorStopAt(0, QColor(0, 0, 255));
      setColorStopAt(1, QColor(255, 250, 250));
      break;
    case gpGeography:
      setColorInterpolation(ciRGB);
      setColorStopAt(0, QColor(70, 170, 210));
      setColorStopAt(0.20, QColor(90, 160, 180));
      setColorStopAt(0.25, QColor(45, 130, 175));
      setColorStopAt(0.30, QColor(100, 140, 125));
      setColorStopAt(0.5, QColor(100, 140, 100));
      setColorStopAt(0.6, QColor(130, 145, 120));
      setColorStopAt(0.7, QColor(140, 130, 120));
      setColorStopAt(0.9, QColor(180, 190, 190));
      setColorStopAt(1, QColor(210, 210, 230));
      break;
    case gpIon:
      setColorInterpolation(ciHSV);
      setColorStopAt(0, QColor(60, 45, 20));
      setColorStopAt(0.45, QColor(0, 0, 255));
      setColorStopAt(0.8, QColor(0, 255, 255));
      setColorStopAt(1, QColor(0, 255, 0));
      break;
    case gpPolar:
      setColorInterpolation(ciRGB);
      setColorStopAt(0, QColor(50, 255, 255));
      setColorStopAt(0.18, QColor(10, 70, 255));
      setColorStopAt(0.28, QColor(10, 10, 190));
      setColorStopAt(0.5, QColor(0, 0, 0));
      setColorStopAt(0.72, QColor(190, 10, 10));
      setColorStopAt(0.82, QColor(255, 70, 10));
      setColorStopAt(1, QColor(255, 255, 50));
      break;
    case gpSpectrum:
      setColorInterpolation(ciHSV);
      setColorStopAt(0, QColor(50, 0, 50));
      setColorStopAt(0.15, QColor(0, 0, 255));
      setColorStopAt(0.35, QColor(0, 255, 255));
      setColorStopAt(0.6, QColor(255, 255, 0));
      setColorStopAt(0.75, QColor(255, 30, 0));
      setColorStopAt(1, QColor(50, 0, 0));
      break;
    case gpJet:
      setColorInterpolation(ciRGB);
      setColorStopAt(0, QColor(0, 0, 100));
      setColorStopAt(0.15, QColor(0, 50, 255));
      setColorStopAt(0.35, QColor(0, 255, 255));
      setColorStopAt(0.65, QColor(255, 255, 0));
      setColorStopAt(0.85, QColor(255, 30, 0));
      setColorStopAt(1, QColor(100, 0, 0));
      break;
    case gpHues:
      setColorInterpolation(ciHSV);
      setColorStopAt(0, QColor(255, 0, 0));
      setColorStopAt(1.0/3.0, QColor(0, 255, 0));
      setColorStopAt(2.0/3.0, QColor(0, 0, 255));
      setColorStopAt(1, QColor(255, 0, 0));
      break;
  }
}

void QCPColorGradient::clearColorStops()
{
  mColorStops.clear();
  mColorBufferInvalidated = true;
}

QCPColorGradient QCPColorGradient::inverted() const
{
  QCPColorGradient result(*this);
  result.clearColorStops();
  for (QMap<double, QColor>::const_iterator it=mColorStops.constBegin(); it!=mColorStops.constEnd(); ++it)
    result.setColorStopAt(1.0-it.key(), it.value());
  return result;
}

void QCPColorGradient::updateColorBuffer()
{
  if (mColorBuffer.size() != mLevelCount)
    mColorBuffer.resize(mLevelCount);
  if (mColorStops.size() > 1)
  {
    double indexToPosFactor = 1.0/(double)(mLevelCount-1);
    for (int i=0; i<mLevelCount; ++i)
    {
      double position = i*indexToPosFactor;
      QMap<double, QColor>::const_iterator it = mColorStops.lowerBound(position);
      if (it == mColorStops.constEnd()) // position is on or after last stop, use color of last stop
      {
        mColorBuffer[i] = (it-1).value().rgb();
      } else if (it == mColorStops.constBegin()) // position is on or before first stop, use color of first stop
      {
        mColorBuffer[i] = it.value().rgb();
      } else // position is in between stops (or on an intermediate stop), interpolate color
      {
        QMap<double, QColor>::const_iterator high = it;
        QMap<double, QColor>::const_iterator low = it-1;
        double t = (position-low.key())/(high.key()-low.key()); // interpolation factor 0..1
        switch (mColorInterpolation)
        {
          case ciRGB:
          {
            mColorBuffer[i] = qRgb((1-t)*low.value().red() + t*high.value().red(),
                                   (1-t)*low.value().green() + t*high.value().green(),
                                   (1-t)*low.value().blue() + t*high.value().blue());
            break;
          }
          case ciHSV:
          {
            QColor lowHsv = low.value().toHsv();
            QColor highHsv = high.value().toHsv();
            double hue = 0;
            double hueDiff = highHsv.hueF()-lowHsv.hueF();
            if (hueDiff > 0.5)
              hue = lowHsv.hueF() - t*(1.0-hueDiff);
            else if (hueDiff < -0.5)
              hue = lowHsv.hueF() + t*(1.0+hueDiff);
            else
              hue = lowHsv.hueF() + t*hueDiff;
            if (hue < 0) hue += 1.0;
            else if (hue >= 1.0) hue -= 1.0;
            mColorBuffer[i] = QColor::fromHsvF(hue, (1-t)*lowHsv.saturationF() + t*highHsv.saturationF(), (1-t)*lowHsv.valueF() + t*highHsv.valueF()).rgb();
            break;
          }
        }
      }
    }
  } else if (mColorStops.size() == 1)
  {
    mColorBuffer.fill(mColorStops.constBegin().value().rgb());
  } else // mColorStops is empty, fill color buffer with black
  {
    mColorBuffer.fill(qRgb(0, 0, 0));
  }
  mColorBufferInvalidated = false;
}

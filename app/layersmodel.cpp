/***************************************************************************
  qgsquicklayertreemodel.cpp
  --------------------------------------
  Date                 : Nov 2017
  Copyright            : (C) 2017 by Peter Petrik
  Email                : zilolv at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "layersmodel.h"
#include "qgsquickmapsettings.h"

#include <qgslayertreemodel.h>
#include <qgslayertreenode.h>
#include <qgslayertree.h>
#include <qgsvectorlayer.h>
#include <qgslayertreemodellegendnode.h>
#include <qgsproject.h>
#include "qgsvectorlayer.h"
#include <qgsmaplayer.h>
#include <qgswkbtypes.h>

#include <QString>

LayersModel::LayersModel( QObject *parent )
  : QAbstractListModel( parent )
{
}

LayersModel::~LayersModel()
{
}


void LayersModel::reloadLayers( QgsProject *project )
{

  if ( !project ) return;

  QgsLayerTreeGroup *root = project->layerTreeRoot();

  // Get list of all visible and valid layers in the project
  QList< QgsMapLayer * > allLayers;
  foreach ( QgsLayerTreeLayer *nodeLayer, root->findLayers() )
  {
    if ( nodeLayer->isVisible() )
    {
      QgsMapLayer *layer = nodeLayer->layer();
      if ( layer->isValid() )
      {
        allLayers << layer;
        qDebug() << "Found layer: " << layer->name();
      }
    }
  }
  if ( mLayers != allLayers )
  {
    beginResetModel();
    setLayers( allLayers );
    endResetModel();
  }
}

QgsQuickMapSettings *LayersModel::mapSettings() const
{
  return mMapSettings;
}

void LayersModel::setMapSettings( QgsQuickMapSettings *mapSettings )
{
  if ( mapSettings != mMapSettings )
  {
    mMapSettings = mapSettings;
    setLayers( mLayers );
    emit mapSettingsChanged();
  }
}

void LayersModel::updateActiveLayer( const QString &name )
{
  int row = rowAccordingName( name, firstWritableLayerIndex() );
  setActiveIndex( row );
}

int LayersModel::activeIndex() const
{
  return mActiveIndex;
}

QString LayersModel::setActiveIndex( int activeIndex )
{
  if ( mActiveIndex != activeIndex )
  {
    mActiveIndex = activeIndex;
  }
  emit activeIndexChanged();

  QgsMapLayer *layer = activeLayer();
  if ( layer )
    return layer->name();
  else
    return QString();
}

QVariant LayersModel::data( const QModelIndex &index, int role ) const
{
  int row = index.row();
  if ( row < 0 || row >= mLayers.count() )
    return QVariant( "" );

  QgsMapLayer *layer = mLayers.at( row );

  switch ( role )
  {
    case Name:
    {
      return layer->name();
    }
    case isVector:
    {
      return layer->type() == QgsMapLayerType::VectorLayer;
    }
    case isReadOnly:
    {
      return layer->readOnly();
    }
    case IconSource:
    {
      QgsVectorLayer *vectorLayer = qobject_cast<QgsVectorLayer *>( layer );
      if ( vectorLayer )
      {
        QgsWkbTypes::GeometryType type = vectorLayer->geometryType();
        switch ( type )
        {
          case QgsWkbTypes::GeometryType::PointGeometry: return "mIconPointLayer.svg";
          case QgsWkbTypes::GeometryType::LineGeometry: return "mIconLineLayer.svg";
          case QgsWkbTypes::GeometryType::PolygonGeometry: return "mIconPolygonLayer.svg";
          case QgsWkbTypes::GeometryType::UnknownGeometry: return "";
          case QgsWkbTypes::GeometryType::NullGeometry: return "";
        }
        return QVariant();
      }
      else return "mIconRaster.svg";
    }
    case VectorLayer:
    {
      QgsVectorLayer *vectorLayer = qobject_cast<QgsVectorLayer *>( layer );
      if ( vectorLayer )
      {
        return QVariant::fromValue<QgsVectorLayer *>( vectorLayer );
      }
      return QVariant();
    }
    case HasGeometry:
    {
      QgsVectorLayer *vectorLayer = qobject_cast<QgsVectorLayer *>( layer );
      if ( vectorLayer )
      {
        return vectorLayer->wkbType() != QgsWkbTypes::NoGeometry && vectorLayer->wkbType() != QgsWkbTypes::Unknown;
      }
      return false;
    }
  }

  return QVariant();
}

QHash<int, QByteArray> LayersModel::roleNames() const
{
  QHash<int, QByteArray> roleNames = QAbstractListModel::roleNames();
  roleNames[Name] = "name";
  roleNames[isVector] = "isVector";
  roleNames[isReadOnly] = "isReadOnly";
  roleNames[IconSource] = "iconSource";
  roleNames[VectorLayer] = "vectorLayer";
  roleNames[HasGeometry] = "hasGeometry";
  return roleNames;
}

QModelIndex LayersModel::index( int row, int column, const QModelIndex &parent ) const
{
  Q_UNUSED( column )
  Q_UNUSED( parent )
  return createIndex( row, 0, nullptr );
}

int LayersModel::rowAccordingName( QString name, int defaultRow ) const
{
  int i = 0;
  for ( QgsMapLayer *layer : mLayers )
  {
    if ( layer->name() == name )
    {
      return i;
    }
    i++;
  }
  return defaultRow;
}

int LayersModel::noOfEditableLayers() const
{
  int count = 0;
  for ( QgsMapLayer *layer : mLayers )
  {
    if ( !layer->readOnly() )
    {
      count++;
    }
  }

  return count;
}

int LayersModel::firstWritableLayerIndex() const
{
  int i = 0;
  for ( QgsMapLayer *layer : mLayers )
  {
    if ( !layer->readOnly() )
    {
      return i;
    }
    i++;
  }

  return -1;
}

QgsMapLayer *LayersModel::activeLayer()
{
  if ( mActiveIndex < 0 || mActiveIndex >= mLayers.length() )
    return nullptr;
  return mLayers.at( activeIndex() );
}

int LayersModel::rowCount( const QModelIndex &parent ) const
{
  Q_UNUSED( parent )
  return mLayers.count();
}

void LayersModel::setLayers( QList<QgsMapLayer *> layers )
{
  mLayers = layers;
  if ( mMapSettings )
    mMapSettings->setLayers( layers );
}

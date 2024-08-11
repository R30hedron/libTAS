/*
    Copyright 2015-2024 Clément Gallet <clement.gallet@ens-lyon.org>

    This file is part of libTAS.

    libTAS is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libTAS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with libTAS.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "RamWatchModel.h"

#include "ramsearch/RamWatchDetailed.h"

#include <QtWidgets/QMessageBox>
#include <stdint.h>

RamWatchModel::RamWatchModel(QObject *parent) : QAbstractTableModel(parent) {}

int RamWatchModel::rowCount(const QModelIndex & /*parent*/) const
{
   return ramwatches.size();
}

int RamWatchModel::columnCount(const QModelIndex & /*parent*/) const
{
    return 3;
}

QVariant RamWatchModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            switch (section) {
            case 0:
                return QString("Address");
            case 1:
                return QString("Value");
            case 2:
                return QString("Label");
            }
        }
    }
    return QVariant();
}

QVariant RamWatchModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        const std::unique_ptr<RamWatchDetailed> &watch = ramwatches.at(index.row());
        switch(index.column()) {
            case 0:
                if (watch->isPointer)
                    return QString("P->%1").arg(watch->address, 0, 16);
                else
                    return QString("%1").arg(watch->address, 0, 16);
            case 1:
                return QString(watch->value_str());
            case 2:
                return QString(watch->label.c_str());
            default:
                return QString();
        }
    }
    return QVariant();
}

void RamWatchModel::addWatch(std::unique_ptr<RamWatchDetailed> ramwatch)
{
    beginInsertRows(QModelIndex(), ramwatches.size(), ramwatches.size());
    ramwatches.push_back(std::move(ramwatch));
    endInsertRows();
}

void RamWatchModel::removeWatch(int row)
{
    beginRemoveRows(QModelIndex(), row, row);
    ramwatches.erase(ramwatches.begin() + row);
    endRemoveRows();
}

void RamWatchModel::saveSettings(QSettings& watchSettings)
{
    watchSettings.beginWriteArray("watches");
    int i = 0;
    for (const std::unique_ptr<RamWatchDetailed>& w : ramwatches) {
        watchSettings.setArrayIndex(i++);
        watchSettings.setValue("address", static_cast<unsigned long long>(w->address));
        watchSettings.setValue("label", w->label.c_str());
        watchSettings.setValue("type", w->value_type);
        watchSettings.setValue("hex", w->hex);
        watchSettings.setValue("isPointer", w->isPointer);
        if (w->isPointer) {
            watchSettings.setValue("base_file", w->base_file.c_str());
            watchSettings.setValue("base_file_offset", static_cast<long long>(w->base_file_offset));
            watchSettings.beginWriteArray("offsets");
            int j = 0;
            for (int o : w->pointer_offsets) {
                watchSettings.setArrayIndex(j++);
                watchSettings.setValue("offset", o);
            }
            watchSettings.endArray();
        }
    }
    watchSettings.endArray();
}

void RamWatchModel::loadSettings(QSettings& watchSettings)
{
    beginResetModel();

    int size = watchSettings.beginReadArray("watches");
    ramwatches.clear();
    for (int i = 0; i < size; ++i) {
        watchSettings.setArrayIndex(i);

        std::unique_ptr<RamWatchDetailed> ramwatch;
        int type = watchSettings.value("type").toInt();
        uintptr_t addr = watchSettings.value("address").toULongLong();

        /* Build the ram watch using the right type as template */
        ramwatch.reset(new RamWatchDetailed(addr, type));

        ramwatch->label = watchSettings.value("label").toString().toStdString();
        ramwatch->hex = watchSettings.value("hex").toBool();
        ramwatch->isPointer = watchSettings.value("isPointer").toBool();
        if (ramwatch->isPointer) {
            ramwatch->base_address = 0;
            ramwatch->base_file = watchSettings.value("base_file").toString().toStdString();
            ramwatch->base_file_offset = watchSettings.value("base_file_offset").toLongLong();
            int size_off = watchSettings.beginReadArray("offsets");
            for (int j = 0; j < size_off; ++j) {
                watchSettings.setArrayIndex(j);
                ramwatch->pointer_offsets.push_back(watchSettings.value("offset").toInt());
            }
            watchSettings.endArray();
            ramwatch->update_addr();
        }
        ramwatches.push_back(std::move(ramwatch));
    }
    watchSettings.endArray();

    endResetModel();
}


void RamWatchModel::update()
{
    emit dataChanged(index(0,0), index(rowCount()-1,1), QVector<int>(Qt::DisplayRole));
}

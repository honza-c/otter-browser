/**************************************************************************
* Otter Browser: Web browser controlled by the user, not vice-versa.
* Copyright (C) 2015 - 2017 Michal Dutkiewicz aka Emdek <michal@emdek.pl>
* Copyright (C) 2017 Jan Bajer aka bajasoft <jbajer@gmail.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
**************************************************************************/

#include "HistoryModel.h"
#include "Console.h"
#include "JsonSettings.h"
#include "SessionsManager.h"
#include "ThemesManager.h"
#include "Utils.h"

#include <QtCore/QFile>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>

namespace Otter
{

HistoryEntryItem::HistoryEntryItem() : QStandardItem()
{
}

void HistoryEntryItem::setData(const QVariant &value, int role)
{
	if (model() && qobject_cast<HistoryModel*>(model()))
	{
		model()->setData(index(), value, role);
	}
	else
	{
		QStandardItem::setData(value, role);
	}
}

void HistoryEntryItem::setItemData(const QVariant &value, int role)
{
	QStandardItem::setData(value, role);
}

QString HistoryEntryItem::getTitle() const
{
	return (data(HistoryModel::TitleRole).isNull() ? QCoreApplication::translate("Otter::HistoryEntryItem", "(Untitled)") : data(HistoryModel::TitleRole).toString());
}

QUrl HistoryEntryItem::getUrl() const
{
	return data(HistoryModel::UrlRole).toUrl();
}

QDateTime HistoryEntryItem::getTimeVisited() const
{
	return data(HistoryModel::TimeVisitedRole).toDateTime();
}

QIcon HistoryEntryItem::getIcon() const
{
	return (data(Qt::DecorationRole).isNull() ? ThemesManager::createIcon(QLatin1String("text-html")) : data(Qt::DecorationRole).value<QIcon>());
}

quint64 HistoryEntryItem::getIdentifier() const
{
	return data(HistoryModel::IdentifierRole).toULongLong();
}

HistoryModel::HistoryModel(const QString &path, HistoryType type, QObject *parent) : QStandardItemModel(parent),
	m_type(type)
{
	QFile file(path);

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		Console::addMessage(tr("Failed to open history file: %1").arg(file.errorString()), Console::OtherCategory, Console::ErrorLevel, path);

		return;
	}

	const QJsonArray historyArray(QJsonDocument::fromJson(file.readAll()).array());

	file.close();

	for (int i = 0; i < historyArray.count(); ++i)
	{
		const QJsonObject entryObject(historyArray.at(i).toObject());

		addEntry(QUrl(entryObject.value(QLatin1String("url")).toString()), entryObject.value(QLatin1String("title")).toString(), QIcon(), QDateTime::fromString(entryObject.value(QLatin1String("time")).toString(), Qt::ISODate));
	}

	setSortRole(TimeVisitedRole);
	sort(0, Qt::DescendingOrder);
}

void HistoryModel::clearExcessEntries(int limit)
{
	if (limit > 0 && rowCount() > limit)
	{
		for (int i = (rowCount() - 1); i >= limit; --i)
		{
			removeEntry(index(i, 0).data(HistoryModel::IdentifierRole).toULongLong());
		}
	}
}

void HistoryModel::clearRecentEntries(uint period)
{
	if (period == 0)
	{
		clear();

		emit cleared();

		return;
	}

	for (int i = (rowCount() - 1); i >= 0; --i)
	{
		if (index(i, 0).data(TimeVisitedRole).toDateTime().secsTo(QDateTime::currentDateTime()) < (period * 3600))
		{
			removeEntry(index(i, 0).data(IdentifierRole).toULongLong());
		}
	}
}

void HistoryModel::clearOldestEntries(int period)
{
	if (period < 0)
	{
		return;
	}

	const QDateTime currentDateTime(QDateTime::currentDateTime());

	for (int i = (rowCount() - 1); i >= 0; --i)
	{
		if (index(i, 0).data(TimeVisitedRole).toDateTime().daysTo(currentDateTime) > period)
		{
			removeEntry(index(i, 0).data(IdentifierRole).toULongLong());
		}
	}
}

void HistoryModel::removeEntry(quint64 identifier)
{
	HistoryEntryItem *entry(getEntry(identifier));

	if (!entry)
	{
		return;
	}

	const QUrl url(Utils::normalizeUrl(entry->getUrl()));

	if (m_urls.contains(url))
	{
		m_urls[url].removeAll(entry);

		if (m_urls[url].isEmpty())
		{
			m_urls.remove(url);
		}
	}

	if (identifier > 0 && m_identifiers.contains(identifier))
	{
		m_identifiers.remove(identifier);
	}

	emit entryRemoved(entry);

	removeRow(entry->row());

	emit modelModified();
}

HistoryEntryItem* HistoryModel::addEntry(const QUrl &url, const QString &title, const QIcon &icon, const QDateTime &date, quint64 identifier)
{
	blockSignals(true);

	if (m_type == TypedHistory)
	{
		const QUrl normalizedUrl(Utils::normalizeUrl(url));

		if (hasEntry(normalizedUrl))
		{
			for (int i = 0; i < m_urls[normalizedUrl].count(); ++i)
			{
				removeEntry(m_urls[normalizedUrl][i]->getIdentifier());
			}
		}
	}

	HistoryEntryItem *entry(new HistoryEntryItem());
	entry->setIcon(icon);

	insertRow(0, entry);
	setData(entry->index(), url, UrlRole);
	setData(entry->index(), title, TitleRole);
	setData(entry->index(), date, TimeVisitedRole);

	if (identifier == 0 || m_identifiers.contains(identifier))
	{
		identifier = (m_identifiers.isEmpty() ? 1 : (m_identifiers.keys().last() + 1));
	}

	setData(entry->index(), identifier, IdentifierRole);

	m_identifiers[identifier] = entry;

	blockSignals(false);

	emit entryAdded(entry);

	return entry;
}

HistoryEntryItem* HistoryModel::getEntry(quint64 identifier) const
{
	if (m_identifiers.contains(identifier))
	{
		return m_identifiers[identifier];
	}

	return nullptr;
}

QVector<HistoryModel::HistoryEntryMatch> HistoryModel::findEntries(const QString &prefix, bool markAsTypedIn) const
{
	QVector<HistoryEntryItem*> matchedEntries;
	QVector<HistoryModel::HistoryEntryMatch> allMatches;
	QVector<HistoryModel::HistoryEntryMatch> currentMatches;
	QMultiMap<QDateTime, HistoryModel::HistoryEntryMatch> matchesMap;
	QHash<QUrl, QVector<HistoryEntryItem*> >::const_iterator urlsIterator;

	for (urlsIterator = m_urls.constBegin(); urlsIterator != m_urls.constEnd(); ++urlsIterator)
	{
		if (urlsIterator.value().isEmpty() || matchedEntries.contains(urlsIterator.value().first()))
		{
			continue;
		}

		const QString result(Utils::matchUrl(urlsIterator.key(), prefix));

		if (!result.isEmpty())
		{
			HistoryEntryMatch match;
			match.entry = urlsIterator.value().first();
			match.match = result;

			if (markAsTypedIn)
			{
				match.isTypedIn = true;
			}

			matchesMap.insert(match.entry->data(TimeVisitedRole).toDateTime(), match);

			matchedEntries.append(match.entry);
		}
	}

	currentMatches = matchesMap.values().toVector();

	matchesMap.clear();

	for (int i = (currentMatches.count() - 1); i >= 0; --i)
	{
		allMatches.append(currentMatches.at(i));
	}

	return allMatches;
}

HistoryModel::HistoryType HistoryModel::getType() const
{
	return m_type;
}

bool HistoryModel::save(const QString &path) const
{
	if (SessionsManager::isReadOnly())
	{
		return false;
	}

	QJsonArray historyArray;

	for (int i = 0; i < rowCount(); ++i)
	{
		const QModelIndex index(this->index(i, 0));

		if (index.isValid())
		{
			historyArray.prepend(QJsonObject({{QLatin1String("url"), index.data(UrlRole).toUrl().toString()}, {QLatin1String("title"), index.data(TitleRole).toString()}, {QLatin1String("time"), index.data(TimeVisitedRole).toDateTime().toString(Qt::ISODate)}}));
		}
	}

	JsonSettings settings;
	settings.setArray(historyArray);

	return settings.save(path);
}

bool HistoryModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	HistoryEntryItem *entry(static_cast<HistoryEntryItem*>(itemFromIndex(index)));

	if (!entry)
	{
		return QStandardItemModel::setData(index, value, role);
	}

	if (role == UrlRole && value.toUrl() != index.data(UrlRole).toUrl())
	{
		const QUrl oldUrl(Utils::normalizeUrl(index.data(UrlRole).toUrl()));
		const QUrl newUrl(Utils::normalizeUrl(value.toUrl()));

		if (!oldUrl.isEmpty() && m_urls.contains(oldUrl))
		{
			m_urls[oldUrl].removeAll(entry);

			if (m_urls[oldUrl].isEmpty())
			{
				m_urls.remove(oldUrl);
			}
		}

		if (!newUrl.isEmpty())
		{
			if (!m_urls.contains(newUrl))
			{
				m_urls[newUrl] = QVector<HistoryEntryItem*>();
			}

			m_urls[newUrl].append(entry);
		}
	}

	entry->setItemData(value, role);

	switch (role)
	{
		case TitleRole:
		case UrlRole:
		case IdentifierRole:
		case TimeVisitedRole:
			emit entryModified(entry);
			emit modelModified();

			break;
		default:
			break;
	}

	return true;
}

bool HistoryModel::hasEntry(const QUrl &url) const
{
	return m_urls.contains(url);
}

}

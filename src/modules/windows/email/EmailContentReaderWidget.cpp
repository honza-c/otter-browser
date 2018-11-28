/**************************************************************************
* Otter Browser: Web browser controlled by the user, not vice-versa.
* Copyright (C) 2014 Piotr Wójcik <chocimier@tlen.pl>
* Copyright (C) 2014 - 2017 Michal Dutkiewicz aka Emdek <michal@emdek.pl>
* Copyright (C) 2018 Jan Čulík <jan@culik.net>
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

#include "EmailContentReaderWidget.h"
#include "ui_EmailContentReaderWidget.h"

namespace Otter
{

EmailContentReaderWidget::EmailContentReaderWidget(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::EmailContentReaderWidget)
{
    m_ui->setupUi(this);

    setupTableModel();
    setupTableView();
}

EmailContentReaderWidget::~EmailContentReaderWidget()
{
    delete m_ui;
}

void EmailContentReaderWidget::setupTableModel()
{
    m_messageMetadataTableModel = new MessageMetadataSqlTableModel();
    m_messageMetadataTableModel->setTable("MessageData");
    m_messageMetadataTableModel->select();
}

void EmailContentReaderWidget::setupTableView()
{
    m_ui->messageMetadataTableView->setModel(m_messageMetadataTableModel);

    m_ui->messageMetadataTableView->horizontalHeader()->moveSection(11, 0);
    m_ui->messageMetadataTableView->horizontalHeader()->moveSection(10, 1);
    m_ui->messageMetadataTableView->horizontalHeader()->moveSection(11, 2);

    m_ui->messageMetadataTableView->hideColumn(1);
    m_ui->messageMetadataTableView->hideColumn(2);
    m_ui->messageMetadataTableView->hideColumn(3);
    m_ui->messageMetadataTableView->hideColumn(4);
    m_ui->messageMetadataTableView->hideColumn(5);
    m_ui->messageMetadataTableView->hideColumn(6);
    m_ui->messageMetadataTableView->hideColumn(7);
    m_ui->messageMetadataTableView->hideColumn(12);
    m_ui->messageMetadataTableView->hideColumn(13);
    m_ui->messageMetadataTableView->hideColumn(14);
    m_ui->messageMetadataTableView->hideColumn(15);
    m_ui->messageMetadataTableView->hideColumn(16);
    m_ui->messageMetadataTableView->hideColumn(17);
    m_ui->messageMetadataTableView->hideColumn(18);
    m_ui->messageMetadataTableView->hideColumn(19);

    m_ui->messageMetadataTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_ui->messageMetadataTableView->verticalHeader()->hide();
    m_ui->messageMetadataTableView->horizontalHeader()->moveSection(3, 0);

    m_ui->messageMetadataTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_ui->messageMetadataTableView->setSelectionMode(QTableView::SingleSelection);
    m_ui->messageMetadataTableView->setShowGrid(false);


    m_ui->messageMetadataTableView->horizontalHeader()->setStretchLastSection(false);

    m_ui->messageMetadataTableView->setColumnWidth(0, 70); // id
    m_ui->messageMetadataTableView->horizontalHeader()->setSectionResizeMode(11, QHeaderView::Stretch); // subject
    m_ui->messageMetadataTableView->setColumnWidth(9, 250); // sender
    m_ui->messageMetadataTableView->setColumnWidth(10, 100); // size
    m_ui->messageMetadataTableView->setColumnWidth(8, 150); // date
}

}

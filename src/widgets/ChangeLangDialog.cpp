#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QDir>
#include <QFileInfo>
#include <QCoreApplication>
#include <QSettings>

#include "ChangeLangDialog.h"

ChangeLangDialog::ChangeLangDialog(QWidget *parent)
    : QDialog(parent)
    , m_lang(new QComboBox(this))
    , m_layout(new QVBoxLayout(this))
    , m_buttons_layout(new QHBoxLayout())
    , m_accept(new QPushButton(tr("Accept"), this))
    , m_cancel(new QPushButton(tr("Cancel"), this))
{
    this->setWindowTitle(tr("Change Language"));
    m_layout->addWidget(new QLabel(tr("Language"), this));
    m_layout->addWidget(m_lang);
    m_buttons_layout->addWidget(m_accept, 0, Qt::AlignRight);
    m_buttons_layout->addWidget(m_cancel, 0, Qt::AlignRight);
    m_layout->addLayout(m_buttons_layout);

    QDir dir(qApp->applicationDirPath() + "/ts/");
    QFileInfoList files = dir.entryInfoList({"*.qm"}, QDir::Files);

    for(const auto &file : files) {
        m_lang->addItem(file.baseName());
    }

    connect(m_cancel, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_accept, &QPushButton::clicked, this, [&]{
                    QSettings settings(qApp->applicationDirPath() + "/settings.ini", QSettings::IniFormat);
                    settings.setValue("Locale", m_lang->currentText());
                    this->accept();
            });
}

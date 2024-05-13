#pragma once

#include <QDialog>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

class ChangeLangDialog : public QDialog
{
    Q_OBJECT
    public:
        ChangeLangDialog(QWidget *parent = nullptr);

    private:
        QVBoxLayout *m_layout;
        QHBoxLayout *m_buttons_layout;
        QComboBox *m_lang;

        QPushButton *m_accept;
        QPushButton *m_cancel;
};

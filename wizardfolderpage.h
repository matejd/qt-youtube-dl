#ifndef FOLDERWIZARDPAGE_H
#define FOLDERWIZARDPAGE_H

#include "wizardcommon.h"

#include <QWizardPage>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QSettings>

class WizardFolderPage : public QWizardPage
{
    Q_OBJECT

private:
    QLineEdit* m_pathLineEdit = nullptr;

public:
    explicit WizardFolderPage(QWidget* p = nullptr): QWizardPage(p)
    {
        setSubTitle("Select the destination folder.");

        m_pathLineEdit = new QLineEdit();
        m_pathLineEdit->setReadOnly(true);
        registerField(FIELD_FOLDER, m_pathLineEdit);
        QPushButton* pathButton = new QPushButton("Open");
        connect(pathButton, SIGNAL(clicked()), this, SLOT(openButtonClicked()));
        const QSettings persistentSettings;
        m_pathLineEdit->setText(persistentSettings.value(SETTING_DEST_FOLDER, "").toString());

        QHBoxLayout* layout = new QHBoxLayout();
        layout->addWidget(m_pathLineEdit);
        layout->addWidget(pathButton);
        setLayout(layout);
    }

    void initializePage() override
    {
        // Since fields are reset on wizard resets, this is a good time
        // to refresh them.
        const QSettings persistentSettings;
        m_pathLineEdit->setText(persistentSettings.value(SETTING_DEST_FOLDER, "").toString());
    }

private slots:
    void openButtonClicked()
    {
        const QString dir = QFileDialog::getExistingDirectory(this, "Select directory", "",
                                                        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        if (dir.size() == 0)
            return;
        m_pathLineEdit->setText(dir);
        QSettings persistentSettings;
        persistentSettings.setValue(SETTING_DEST_FOLDER, dir);
    }
};

#endif // FOLDERWIZARDPAGE_H

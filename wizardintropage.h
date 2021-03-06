#ifndef WIZARDINTRO_H
#define WIZARDINTRO_H

#include "wizardcommon.h"

#include <QWizardPage>
#include <QLabel>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QApplication>
#include <QClipboard>
#include <QUrl>
#include <QSettings>

class WizardIntroPage : public QWizardPage
{
    Q_OBJECT

private:
    QLineEdit* m_urlLineEdit = nullptr;
    QCheckBox* m_formatCheckBox = nullptr;
    QCheckBox* m_sameFolderCheckBox = nullptr;

public:
    explicit WizardIntroPage(QWidget* p = nullptr): QWizardPage(p)
    {
        setSubTitle("Paste the URL of the page with video you"\
                    " want to download. You can also" \
                    " optionally skip step 2 (video format"\
                    " selection) and step 3 (download folder).");
        QLabel* urlLabel = new QLabel();
        urlLabel->setText("URL:");
        m_urlLineEdit = new QLineEdit();
        registerField(FIELD_URL_REGISTER, m_urlLineEdit);

        m_formatCheckBox = new QCheckBox("Pick the best available format");
        m_sameFolderCheckBox = new QCheckBox("Same destination folder as last time");
        registerField(FIELD_SAME_FOLDER, m_sameFolderCheckBox);

        connect(m_formatCheckBox,     SIGNAL(clicked()), this, SLOT(onCheckboxChecked()));
        connect(m_sameFolderCheckBox, SIGNAL(clicked()), this, SLOT(onCheckboxChecked()));

        QHBoxLayout* urlLayout = new QHBoxLayout();
        urlLayout->addWidget(urlLabel);
        urlLayout->addWidget(m_urlLineEdit);

        QVBoxLayout* mainLayout = new QVBoxLayout();
        mainLayout->addLayout(urlLayout);
        mainLayout->addSpacing(10);
        mainLayout->addWidget(m_formatCheckBox);
        mainLayout->addWidget(m_sameFolderCheckBox);
        setLayout(mainLayout);
    }

    void initializePage() override
    {
        // Paste the link from the clipboard automatically!
        const QClipboard* clipboard = QApplication::clipboard();
        const QString possibleUrlString = clipboard->text();
        const QUrl possibleUrl(possibleUrlString);
        // QUrl.isValid() does not really work. Checking for
        // non-empty host seems to work fine.
        if (possibleUrl.host().size() != 0)
            m_urlLineEdit->setText(possibleUrlString);
        else
            m_urlLineEdit->setText("");

        // First session ever, user has not yet selected a folder.
        const QSettings persistentSettings;
        if (!persistentSettings.contains(SETTING_DEST_FOLDER))
            m_sameFolderCheckBox->setEnabled(false);
        else
            m_sameFolderCheckBox->setEnabled(true);

        m_formatCheckBox->setChecked(persistentSettings.value(SETTING_BEST_FORMAT, true).toBool());
        m_sameFolderCheckBox->setChecked(persistentSettings.value(SETTING_SAME_FOLDER, false).toBool());

        // Fields are reset every time the wizard is started.
        // We need to set "folder" to the current value (otherwise "folder" field
        // is reset to the value on app startup, which can be old).
        setField(FIELD_FOLDER, persistentSettings.value(SETTING_DEST_FOLDER, "").toString());
    }

    int nextId() const override
    {
        const bool bestFormat = m_formatCheckBox->isChecked();
        const bool sameFolder = m_sameFolderCheckBox->isChecked();
        if (bestFormat && sameFolder)
            return -1; // Nothing more to do.
        if (bestFormat)
            return PAGE_ID_FOLDER;
        return PAGE_ID_FORMATS;
    }

private slots:
    void onCheckboxChecked()
    {
        const bool bestFormat = m_formatCheckBox->isChecked();
        const bool sameFolder = m_sameFolderCheckBox->isChecked();
        QSettings persistentSettings;
        persistentSettings.setValue(SETTING_BEST_FORMAT, bestFormat);
        persistentSettings.setValue(SETTING_SAME_FOLDER, sameFolder);

        // When both "best format" and "same folder" are checked,
        // this page is the last page of the wizard.
        if (bestFormat && sameFolder)
            setFinalPage(true);
        else
            setFinalPage(false);
    }
};

#endif

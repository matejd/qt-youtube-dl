#ifndef INTROWIZARDPAGE_H
#define INTROWIZARDPAGE_H

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

class IntroWizardPage : public QWizardPage
{
    Q_OBJECT

private:
    QLineEdit* m_urlLineEdit = nullptr;
    QCheckBox* m_formatCheckBox = nullptr;
    QCheckBox* m_sameFolderCheckBox = nullptr;

public:
    explicit IntroWizardPage(QWidget* p = 0): QWizardPage(p)
    {
        setSubTitle("Paste the URL of the page with video you"\
                    " want to download. You can also" \
                    " optionally skip step 2 (video format"\
                    " selection) and step 3 (download folder).");
        QLabel* urlLabel = new QLabel();
        urlLabel->setText("URL:");
        m_urlLineEdit = new QLineEdit();
        registerField("url*", m_urlLineEdit);

        m_formatCheckBox = new QCheckBox("Pick the best available format");
        m_sameFolderCheckBox = new QCheckBox("Same destination folder as last time");
        registerField("sameFolder", m_sameFolderCheckBox);

        connect(m_formatCheckBox, SIGNAL(clicked()), this, SLOT(onCheckboxChecked()));
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
        if (!persistentSettings.contains("folderWizardPage/destFolder"))
            m_sameFolderCheckBox->setEnabled(false);
        else
            m_sameFolderCheckBox->setEnabled(true);

        m_formatCheckBox->setChecked(persistentSettings.value("introWizardPage/bestFormat", true).toBool());
        m_sameFolderCheckBox->setChecked(persistentSettings.value("introWizardPage/sameFolder", false).toBool());

        // Fields are reset every time the wizard is started.
        // We need to set "folder" to the current value (otherwise "folder" field
        // is reset to the value on app startup, which can be old).
        setField("folder", persistentSettings.value("folderWizardPage/destFolder", "").toString());
    }

    int nextId() const override
    {
        const bool bestFormat = m_formatCheckBox->isChecked();
        const bool sameFolder = m_sameFolderCheckBox->isChecked();
        if (bestFormat && sameFolder)
            return -1; // Nothing more to do.
        if (bestFormat)
            return 2;  // Go to folderWizardPage.
        return 1;      // Go to videoFormatsPage.
    }

private slots:
    void onCheckboxChecked()
    {
        const bool bestFormat = m_formatCheckBox->isChecked();
        const bool sameFolder = m_sameFolderCheckBox->isChecked();
        QSettings persistentSettings;
        persistentSettings.setValue("introWizardPage/bestFormat", bestFormat);
        persistentSettings.setValue("introWizardPage/sameFolder", sameFolder);

        // When both "best format" and "same folder" are checked,
        // this page is the last page of the wizard.
        if (bestFormat && sameFolder)
            setFinalPage(true);
        else
            setFinalPage(false);
    }
};

#endif // INTROWIZARDPAGE_H

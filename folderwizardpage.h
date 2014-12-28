#ifndef FOLDERWIZARDPAGE_H
#define FOLDERWIZARDPAGE_H

#include <QWizardPage>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QSettings>

class FolderWizardPage : public QWizardPage
{
    Q_OBJECT

private:
    QLineEdit* m_pathLineEdit = nullptr;

public:
    explicit FolderWizardPage(QWidget* p = 0): QWizardPage(p)
    {
        setSubTitle("Select the destination folder.");

        m_pathLineEdit = new QLineEdit();
        m_pathLineEdit->setReadOnly(true);
        registerField("folder", m_pathLineEdit);
        QPushButton* pathButton = new QPushButton("Open");
        connect(pathButton, SIGNAL(clicked()), this, SLOT(openButtonClicked()));
        const QSettings persistentSettings;
        m_pathLineEdit->setText(persistentSettings.value("folderWizardPage/destFolder", "").toString());

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
        m_pathLineEdit->setText(persistentSettings.value("folderWizardPage/destFolder", "").toString());
    }

private slots:
    void openButtonClicked()
    {
        const QString dir = QFileDialog::getExistingDirectory(this, "Select directory", "",
                                                        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        m_pathLineEdit->setText(dir);
        QSettings persistentSettings;
        persistentSettings.setValue("folderWizardPage/destFolder", dir);
    }
};

#endif // FOLDERWIZARDPAGE_H

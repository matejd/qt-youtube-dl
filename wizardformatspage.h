#ifndef DOWNLOADWIZARD_H
#define DOWNLOADWIZARD_H

#include "youtubedl.h"
#include "wizardcommon.h"

#include <QAbstractButton>
#include <QWizard>
#include <QWizardPage>
#include <QTimer>
#include <QtConcurrent>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QRadioButton>
#include <QScrollArea>
#include <QGroupBox>
#include <QFontDatabase>

class WizardFormatsPage : public QWizardPage
{
    Q_OBJECT

private:
    QScrollArea* m_scrollArea = nullptr;
    QGroupBox* m_formatsGroupBox = nullptr;
    QLineEdit* m_hiddenFormatEdit = nullptr;
    QMap<QObject*, QString> m_radioFormat; // Mapping radioButton => formatId.

public:
    explicit WizardFormatsPage(QWidget* p = nullptr): QWizardPage(p)
    {
        m_scrollArea = new QScrollArea();
        m_scrollArea->setWidgetResizable(true);
        m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        m_scrollArea->setFrameShape(QFrame::NoFrame);

        // Add a dummy QLineEdit. We'll attach a field to it.
        m_hiddenFormatEdit = new QLineEdit();
        m_hiddenFormatEdit->setHidden(true);
        registerField(FIELD_FORMAT, m_hiddenFormatEdit);
        m_hiddenFormatEdit->setText("best"); // Best video format (default).

        QVBoxLayout* mainLayout = new QVBoxLayout();
        mainLayout->addWidget(m_scrollArea);
        mainLayout->addWidget(m_hiddenFormatEdit);
        setLayout(mainLayout);
    }

    // Called when Next is clicked on the first page.
    void initializePage() override
    {
        setSubTitle("Retrieving video formats...");
        if (m_formatsGroupBox != nullptr) {
            // This *should* delete all children as well (radio buttons).
            delete m_formatsGroupBox;
            m_radioFormat.clear();
        }
        m_formatsGroupBox = new QGroupBox();
        m_formatsGroupBox->setFlat(true);
        m_formatsGroupBox->setLayout(new QVBoxLayout());
        m_formatsGroupBox->layout()->setAlignment(Qt::AlignTop);
        m_scrollArea->setWidget(m_formatsGroupBox);

        // Workaround, disable wizard buttons temporarily.
        QTimer::singleShot(0, this, SLOT(retrieveVideoFormats()));
    }

    int nextId() const override
    {
        if (field(FIELD_SAME_FOLDER).toBool())
            return -1; // Finish.
        return PAGE_ID_FOLDER;
    }

private slots:
    void retrieveVideoFormats()
    {
        QAbstractButton* const backButton = wizard()->button(QWizard::BackButton);
        QAbstractButton* const nextButton = wizard()->button(QWizard::NextButton);
        QAbstractButton* const cancelButton = wizard()->button(QWizard::CancelButton);
        QAbstractButton* const finishButton = wizard()->button(QWizard::FinishButton);
        const QString url = field(FIELD_URL).toString();

        // Temporary disable wizard's buttons, since the network request might take a while.
        backButton->setEnabled(false);
        nextButton->setEnabled(false);
        cancelButton->setEnabled(false);
        finishButton->setEnabled(false);
        QtConcurrent::run([=] {
            // Run in a background thread.
            const QList<VideoFormat> formats = getVideoFormats(url);
            QMetaObject::invokeMethod(this, "formatsAvailable", Qt::QueuedConnection, Q_ARG(QList<VideoFormat>, formats));
        });
    }

    void formatsAvailable(QList<VideoFormat> formats)
    {
        QAbstractButton* const backButton = wizard()->button(QWizard::BackButton);
        QAbstractButton* const nextButton = wizard()->button(QWizard::NextButton);
        QAbstractButton* const cancelButton = wizard()->button(QWizard::CancelButton);
        QAbstractButton* const finishButton = wizard()->button(QWizard::FinishButton);

        if (formats.size() == 0) {
            setSubTitle("Failed to retrieve any available format! Check the URL!");
            backButton->setEnabled(true);
            cancelButton->setEnabled(true);
            // Keep the next/finish button disabled.
            return;
        }
        setSubTitle("Pick the desired video/audio format.");
        const QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);

        bool oneButtonChecked = false;
        for (const VideoFormat& format: formats) {
            QRadioButton* formatRadioButton = new QRadioButton(format.description);
            m_radioFormat[formatRadioButton] = format.id;
            formatRadioButton->setFont(fixedFont);
            if (!oneButtonChecked) {
                formatRadioButton->setChecked(true);
                setField(FIELD_FORMAT, format.id);
                oneButtonChecked = true;
            }
            m_formatsGroupBox->layout()->addWidget(formatRadioButton);
            connect(formatRadioButton, SIGNAL(toggled(bool)), SLOT(formatButtonToggled(bool)));
        }

        backButton->setEnabled(true);
        nextButton->setEnabled(true);
        cancelButton->setEnabled(true);
        finishButton->setEnabled(true); // If visible.
    }

    void formatButtonToggled(bool checked)
    {
        if (!checked)
            return;
        QObject* sender = QObject::sender();
        Q_ASSERT(sender != nullptr);
        Q_ASSERT(m_radioFormat.contains(sender));
        setField(FIELD_FORMAT, m_radioFormat.value(sender));
    }
};

#endif // DOWNLOADWIZARD_H


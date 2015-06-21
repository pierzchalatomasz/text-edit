#include "textedit.h"
#include "ui_textedit.h"
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QDialog>
#include <syntaxhighlighter.h>
#include <QFileDialog>
#include <QBrush>
#include "styles.h"
#include <QFontDialog>
#include <QShortcut>

int fileType;
TextEdit::TextEdit(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TextEdit)
{
    ui->setupUi(this);
    loadFile("test.html");
    syntaxHighlighter();
    setConnections();

    TextEdit::setWindowIcon(QIcon(":/icons/icons/pencil.png"));

    Styles *stylesInit = new Styles();

    ui->menuBar->setStyleSheet("background: #fff; color: #333;");
}

TextEdit::~TextEdit()
{
    delete ui;
}

// Ładuje plik
void TextEdit::loadFile(QString fileName)
{
    if(fileType==1){
    QFile file(fileName);
    file.open(QIODevice::ReadWrite);

    if( !file.error() )
    {
        QTextStream fileStream(&file);
        QString fileString = fileStream.readAll();

        ui->textEdit->setPlainText(fileString);
    }
    else
    {
        QDialog loadFileErrorDialog;
        loadFileErrorDialog.show();
        ui->textEdit->setPlainText("Niestety, nie udało się otworzyć pliku!");
    }
    }
    if(fileType==2){

    }
}

// Aktywuje kolorowanie składni
void TextEdit::syntaxHighlighter()
{
    SyntaxHighlighter* highlighter = new SyntaxHighlighter(ui->textEdit->document());
}
//skraca nazwe pliku
QString TextEdit::cutFileName(QString fileName){
    QString shortFileName;
    int lastIndex;


    lastIndex=fileName.lastIndexOf(QRegExp("/"));

    for(int i=lastIndex+1; i<fileName.length(); i++){
        shortFileName=shortFileName+fileName[i];
    }
    return shortFileName;
}

//otwarcie pliku
void TextEdit::on_actionOpenFile_triggered()
{
    QString fileName = QFileDialog::getOpenFileName();
    QString shortFileName=cutFileName(fileName);
    ui->tabWidget->addTab(new QTextEdit, shortFileName);

    checkFileType(fileName);
    loadFile(fileName);
}
//sprawdzenie typu pliku
void TextEdit::checkFileType(QString fileName)
{
    if(fileName.endsWith("html")||fileName.endsWith("htm")){
        fileType=1;
    }
    else if(fileName.endsWith("css")){
        fileType=2;
    }
}

// Zaznacza linię w której jest kursor (gdy pozycja kursora się zmienia)
void TextEdit::on_textEdit_cursorPositionChanged()
{
    QTextEdit::ExtraSelection lineHighlight;
    lineHighlight.cursor = ui->textEdit->textCursor();
    lineHighlight.format.setProperty(QTextFormat::FullWidthSelection, true);
    QColor color;
    color.setRgb(60, 60, 60);
    QBrush brush(color);
    lineHighlight.format.setBackground(brush);

    QList<QTextEdit::ExtraSelection> extras;
    extras << lineHighlight;
    ui->textEdit->setExtraSelections( extras );

    // Dodawanie tabów wewnątrz elementu
    QTextBlock currentLine = ui->textEdit->textCursor().block();
    if(currentLine.text().isEmpty() && currentLine.userState() == -1)
    {
        QString tab = "\t";
        int tabsNumber = 0;
        int tabsCounter = currentLine.previous().text().indexOf(tab);

        while(tabsCounter >= 0)
        {
            tabsNumber++;
            tabsCounter = currentLine.previous().text().indexOf(tab, tabsCounter + tab.length());
        }

        QString tabs;
        int i = 0;
        while(i < tabsNumber)
        {
            tabs.append("\t");
            i++;
        }
        ui->textEdit->textCursor().insertText(tabs);
    }
}

// stworzenie nowego pliku
void TextEdit::on_actionNewFile_triggered()
{
    QString fileName = QFileDialog::getSaveFileName();
    loadFile(fileName);
    ui->tabWidget->addTab(new QTextEdit, fileName);
}

//Obsługa poleceń z menu edycja
void TextEdit::on_actionMenuPaste_triggered(){
    QApplication::postEvent( ui->textEdit, new QKeyEvent(QEvent::KeyPress, Qt::Key_V, Qt::ControlModifier));
    QApplication::postEvent( ui->textEdit, new QKeyEvent(QEvent::KeyRelease, Qt::Key_V, Qt::ControlModifier));
}

void TextEdit::on_actionMenuCopy_triggered(){
    QApplication::postEvent( ui->textEdit, new QKeyEvent(QEvent::KeyPress, Qt::Key_C, Qt::ControlModifier));
    QApplication::postEvent( ui->textEdit, new QKeyEvent(QEvent::KeyRelease, Qt::Key_C, Qt::ControlModifier));
}

void TextEdit::on_actionMenuCut_triggered(){
    QApplication::postEvent( ui->textEdit, new QKeyEvent(QEvent::KeyPress, Qt::Key_X, Qt::ControlModifier));
    QApplication::postEvent( ui->textEdit, new QKeyEvent(QEvent::KeyRelease, Qt::Key_X, Qt::ControlModifier));
}

void TextEdit::on_actionMenuUndo_triggered(){
    QApplication::postEvent( ui->textEdit, new QKeyEvent(QEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier));
    QApplication::postEvent( ui->textEdit, new QKeyEvent(QEvent::KeyRelease, Qt::Key_Z, Qt::ControlModifier));
}

void TextEdit::on_actionMenuRedo_triggered(){
    QApplication::postEvent( ui->textEdit, new QKeyEvent(QEvent::KeyPress, Qt::Key_Y, Qt::ControlModifier));
    QApplication::postEvent( ui->textEdit, new QKeyEvent(QEvent::KeyRelease, Qt::Key_Y, Qt::ControlModifier));
}

void TextEdit::on_actionMenuSelectAll_triggered(){
    QApplication::postEvent( ui->textEdit, new QKeyEvent(QEvent::KeyPress, Qt::Key_A, Qt::ControlModifier));
    QApplication::postEvent( ui->textEdit, new QKeyEvent(QEvent::KeyRelease, Qt::Key_A, Qt::ControlModifier));
}

// Zamykanie znaczników
void TextEdit::on_textEdit_textChanged()
{
    QTextBlock currentLine = ui->textEdit->textCursor().block();

    currentLine.text();

    QRegExp expression("<div.*>");
    int index = currentLine.text().indexOf(expression);

    QTextBlockUserData *openingsClosings = currentLine.userData();

    if (index >= 0 && currentLine.userState() == -1) {
        currentLine.setUserState(0);
        currentLine.next().setUserState(0);

        QString tab = "\t";
        int tabsNumber = 0;
        int tabsCounter = currentLine.text().indexOf(tab);

        while(tabsCounter >= 0)
        {
            tabsNumber++;
            tabsCounter = currentLine.text().indexOf(tab, tabsCounter + tab.length());
        }

        QString tabs;
        int i = 0;
        while(i < tabsNumber)
        {
            tabs.append("\t");
            i++;
        }

        ui->textEdit->textCursor().insertText("\n\n");
        ui->textEdit->textCursor().insertText(tabs);
        ui->textEdit->textCursor().insertText("</div>");
        ui->textEdit->moveCursor(QTextCursor::Up);
        //ui->textEdit->textCursor().insertText(tabs);
        ui->textEdit->textCursor().insertText("\t");

        currentLine.setUserState(1);
    }
}

// Wybór czcionki
void TextEdit::on_actionMenuFont_triggered(){
    bool font;
    QFont currentFont = ui->textEdit->currentFont();
    QFont selectedFont = QFontDialog::getFont(&font,currentFont,0,"Wybór czcionki");
    if(!font)
        return;
    else
        ui->textEdit->setFont(selectedFont);
}

// Włączenie i wyłączenie kolorowania składni
void TextEdit::on_actionMenuSyntaxHighlighting_triggered(){
    static int mode = 1; // kolorowanie wł = 1, wył = -1
    mode = -mode;

    if(mode == -1){
        foreach(QSyntaxHighlighter* highlighter,
                ui->textEdit->findChildren<QSyntaxHighlighter*>()) {
            delete highlighter;
        }

        QColor defaultcolor;
        defaultcolor.setRgb(60, 60, 60);
        ui->textEdit->setTextColor(defaultcolor);
    }
    else
        syntaxHighlighter();
}

// Obsługa zoom (menu)
void TextEdit::on_actionMenuZoomIn_triggered()
{
    ui->textEdit->zoomIn(2);
}

void TextEdit::on_actionMenuZoomOut_triggered()
{
    ui->textEdit->zoomOut(2);
}

// Aktywuje potrzebne połączenia sygnał -> slot
void TextEdit::setConnections(){
    QShortcut *shortcutZoomIn = new QShortcut(QKeySequence("Ctrl++"),this);
    connect(shortcutZoomIn,SIGNAL(activated()),ui->actionMenuZoomIn,SLOT(trigger()));
    QShortcut *shortcutZoomOut = new QShortcut(QKeySequence("Ctrl+-"),this);
    connect(shortcutZoomOut,SIGNAL(activated()),ui->actionMenuZoomOut,SLOT(trigger()));
    QShortcut *shortcutNewFile = new QShortcut(QKeySequence("Ctrl+N"),this);
    connect(shortcutNewFile,SIGNAL(activated()),ui->actionNewFile,SLOT(trigger()));
    QShortcut *shortcutOpenFile = new QShortcut(QKeySequence("Ctrl+O"),this);
    connect(shortcutOpenFile,SIGNAL(activated()),ui->actionOpenFile,SLOT(trigger()));
}

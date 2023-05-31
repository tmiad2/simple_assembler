#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTemporaryFile>
#include <QProcess>
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

static inline QString get_template(){
    QString out = R"asmembly(
     __attribute__((naked)) void do_it654894851891();
     __attribute__((naked)) void do_it654894851891() {
    __asm__(R"(%1
done43509135123r__:
)");
    }

    int main(){ do_it654894851891(); return 0;}
  )asmembly";
    return out;
}

void MainWindow::on_toolButton_clicked()
{
    auto text = ui->plainTextEdit->toPlainText();
    auto program_text = get_template().arg(text);
    std::cout << program_text.toStdString() << '\n';
    std::flush(std::cout);
    {
    QProcess gcc;
    gcc.start("gcc", QStringList() <<"-xc"  << "-pipe"<< "-o" << "asm_test" << "-masm=intel" << "-" );
    gcc.waitForStarted();
    gcc.write(program_text.toLocal8Bit());
    gcc.closeWriteChannel();
    gcc.waitForFinished();
    std::cout << (gcc.readAllStandardOutput()+gcc.readAllStandardError()).toStdString() << "\n";
    std::flush(std::cout);
    }
    QProcess objdump;
    objdump.start("objdump", QStringList() << "--disassemble=do_it654894851891" << "asm_test");
    objdump.waitForStarted();
    objdump.waitForReadyRead();
    objdump.waitForFinished();
    std::cout << "done!\n\n";
    std::flush(std::cout);
    auto result = objdump.readAllStandardOutput();
    result += objdump.readAllStandardError();

    auto data = result;
    auto lines = data.split('\n');
    bool start=false;
    QStringList useful_lines;
    for(const auto& line:lines){
    if(line.contains("do_it654894851891")){
        start =true;
        continue;
    }
    if(line.contains("done43509135123r__")){
        break;
    }
    if(start){
        useful_lines += line;
    }
    }


    ui->textBrowser->setPlainText(useful_lines.join('\n'));
}


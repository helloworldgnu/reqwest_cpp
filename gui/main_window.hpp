#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>

class MainWindow : public QWidget {
  Q_OBJECT
public:
  MainWindow(QWidget* parent = nullptr);
private slots:
  void test_full();
  void test_download();
};
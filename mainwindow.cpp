#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QMessageBox"
#include <vector>
#include <QLabel>
#include <QInputDialog>
#include <cmath>
#include <QTextStream>
#include <QFile>

QTextStream out(stdout);
QFile file_write("C:/praki/polynomials2/write.txt");
QFile file_read("C:/praki/polynomials2/read.txt");

int counter_polynomials = 0;

int turn_addition = 1;
QTableWidgetItem* first_add_itm = nullptr;
QTableWidgetItem* second_add_itm = nullptr;
QString first_add = "";
QString second_add = "";

int turn_subtraction = 1;
QTableWidgetItem* first_subtract_itm = nullptr;
QTableWidgetItem* second_subtract_itm  = nullptr;
QString first_subtract = "";
QString second_subtract = "";


int turn_multiplication = 1;
QTableWidgetItem* first_multiply_itm = nullptr;
QTableWidgetItem* second_multiply_itm  = nullptr;
polynomial* first_multiply;
polynomial* second_multiply;

int turn_division = 1;
QTableWidgetItem* first_division_itm = nullptr;
QTableWidgetItem* second_division_itm  = nullptr;
polynomial* first_divide;
polynomial* second_divide;

polynomial *base;
polynomial *last;


void CancelAddition() {
    if (first_add_itm != nullptr) first_add_itm->setBackground(Qt::white);
    if (second_add_itm != nullptr) second_add_itm->setBackground(Qt::white);
    first_add_itm = nullptr;
    second_add_itm = nullptr;
    turn_addition = 1;
}

void Cancelsubtraction() {
    if (first_subtract_itm != nullptr) first_subtract_itm->setBackground(Qt::white);
    if (second_subtract_itm != nullptr) second_subtract_itm->setBackground(Qt::white);
    first_subtract_itm = nullptr;
    second_subtract_itm = nullptr;
    turn_subtraction = 1;
}

void CancelMultiplication() {
    if (first_multiply_itm != nullptr) first_multiply_itm->setBackground(Qt::white);
    if (second_multiply_itm != nullptr) second_multiply_itm->setBackground(Qt::white);
    first_multiply_itm = nullptr;
    second_multiply_itm = nullptr;
    turn_multiplication = 1;
}
void CancelDivision() {
    if (first_division_itm != nullptr) first_division_itm->setBackground(Qt::white);
    if (second_division_itm != nullptr) second_division_itm->setBackground(Qt::white);
    first_division_itm = nullptr;
    second_division_itm = nullptr;
    turn_division = 1;
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}

inline void MessageResult(QString &polynomial) {
    QMessageBox::information(0, "Result", polynomial);
    CancelAddition();
    Cancelsubtraction();
    CancelMultiplication();
}

QString PolynomialToString(polynomial* new_base_element) {
    QString result = "";
    QString new_polynomial = "";

    term* tmp = new_base_element->first_term;
    while (tmp  != nullptr) {
        if (tmp->coefficient < 0) {
            new_polynomial += '-';
        } else if (new_polynomial != "") {
            new_polynomial += '+';
        }
        if (abs(tmp->coefficient) != 1) {
            int tmp_coef = abs(tmp->coefficient);
            new_polynomial+=QString::number(tmp_coef);
        } else {
            bool flag = false;
            for (int i = 0; i < 26; ++i) {
                if (tmp ->variables[i] != 0)
                    flag = true;
            }
            if (!flag)
                new_polynomial += '1';
        }
        for (int i = 0; i < 26; ++i) {
            if (tmp->variables[i] == 1) {
                new_polynomial+=('a' + i);
            } else if (tmp ->variables[i] != 0) {
                new_polynomial+=('a' + i);
                new_polynomial+='^' + QString::number(tmp->variables[i]);
            }
        }
        tmp = tmp->next;
    }
    return new_polynomial;
}

bool CheckPolynomial(QString& input) {
    QChar state = 'A';
    if (input[input.size()-1] == '\n') {
        input.resize(input.size()-1);
    }
    try {
        if (input == "") throw(-1);
        for (int i = 0; i < input.size(); ++i) {
            QChar symbol = input[i];
            if (state == 'A') {
                if (symbol <= '9' && symbol >= '0')  state = 'A';
                else if (symbol == '+' || symbol == '-') state = 'C';
                else if (symbol >= 'a' && symbol <= 'z') state = 'D';
                else throw(i);
            } /*else if (state == 'B') {
                if (symbol >= 'a' && symbol <= 'z') state = 'D';
                else if (symbol == '+' || symbol == '-') state = 'C';
                else if (symbol <= '9' && symbol >= '0')  continue;
                else throw(i);
            }*/ else if (state == 'C') {
                if (symbol <= '9' && symbol >= '0')  state = 'A';
                else if (symbol >= 'a' && symbol <= 'z') state = 'D';
                else throw(i);
            } else if (state == 'D') {
                if (symbol >= 'a' && symbol <= 'z') continue;
                else if (symbol == '^') state = 'E';
                else if (symbol == '+' || symbol == '-') state = 'C';
                else throw(i);
            } else if (state == 'E') {
                if (symbol <= '9' && symbol >= '0') state = 'A';
                else throw(i);
            } /*else if (state == 'F') {
                if (symbol <= '9' && symbol >= '0') continue;
                else if (symbol >= 'a' && symbol <= 'z') state = 'D';
                else if (symbol == '+' || symbol == '-') state = 'C';
                else throw(i);
            }*/
        }
        throw(state);
    }  catch (int i) {
        i+=1;
        QString text = "";
        if (state == 'A') {
            text += "A number, a sign or a variable expected in position " + QString::number(i);
        } else if (state == 'C') {
            text += "A number or a variable expected in position " + QString::number(i);
        } else if (state == 'D') {
            text += "A power (^), a sign or a variable expected in position " + QString::number(i);
        } else if (state == 'E') {
            text += "An exponent (number) expected in position " + QString::number(i);
        }
        QMessageBox::critical(0, "Error", text);
        return false;
    } catch (QChar state) {
        if (state == 'A' || state == 'D') return true;
         QString text = "Incomplete input";
         QMessageBox::critical(0, "Error", text);
         return false;
    }

    return true;
}

inline void MessageWrongPolynomial() {
    QMessageBox::critical(0, "Error", "It is not a polynomial");
}

term* GetNewTerm(int &ind, QString& input) {
    term* new_term = new term;
    new_term->coefficient = 0;
    QChar sign = '+';

    if (input[ind] == '+') {
        sign = '+';
        ind++;
    } else if (input[ind] == '-') {
        ind++;
        sign = '-';
    }
    bool flag = true;
    while (ind < input.size() && input[ind] >= '0' && input[ind] <= '9') {
        new_term->coefficient *= 10;
        flag = false;
        new_term->coefficient += (int)(input[ind].unicode() - (int)'0');
        ++ind;
    }
    if (flag)
        new_term -> coefficient = 1;
    while (input[ind] >= 'a' && input[ind] <= 'z') {
        int index = (int)(input[ind].unicode() - (int)'a');
        if (input.size() <= ind + 1 || input[ind + 1] != '^') {
            new_term->variables[index] += 1;
            ind++;
            continue;
        } else {
            ind += 2;
            int tmp_st = 0;
            while (input.size() > ind && input[ind] >= '0' && input[ind] <= '9') {
                int x = (int)(input[ind].unicode() - (int)'0');
                tmp_st = tmp_st * 10 + x;
                ++ind;
            }
            new_term->variables[index] += tmp_st;
        }
    }

    if (sign == '-') new_term->coefficient *= -1;
    return new_term;
}

bool CheckLikeTerms(term* lhs, term* rhs) {
    for (int i = 0; i < 26; ++i) {
        if (lhs->variables[i] != rhs->variables[i])
            return false;
    }
    return true;
}


polynomial* MakeNewPolynomial(QString& input) {
    polynomial* new_polynomial = new polynomial;
    int ind = 0;

    term* last_term;
    term* new_term;

    while (ind < input.size()) {
        new_term = GetNewTerm(ind, input);
        new_term -> next = nullptr;

        if (new_polynomial == nullptr) {
            new_polynomial = new polynomial;
            new_polynomial -> first_term = new_term;
            last_term = new_polynomial->first_term;
            continue;
        }
        if (new_polynomial -> first_term == nullptr) {
            new_polynomial -> first_term = new_term;
            last_term = new_polynomial->first_term;
            continue;
        }
        term* tmp = new_polynomial->first_term;
        bool flag = false;
        for (;;) {
            if (CheckLikeTerms(tmp, new_term)) {
                flag = true;
                tmp->coefficient += new_term->coefficient;
                if (tmp->coefficient == 0) {
                    if (tmp != new_polynomial -> first_term) {
                        tmp->prev->next = tmp->next;
                    } else {
                        new_polynomial -> first_term = new_polynomial -> first_term->next;
                    }
                    if (tmp != last_term && last_term != nullptr)
                        tmp->next->prev = tmp->prev;
                    delete tmp;
                }
                if (tmp != last_term )
                    last_term->next = nullptr;
                delete new_term;
                break;
            }
            if (tmp == last_term)
                break;
            tmp = tmp->next;
        }
        if (!flag) {
            last_term->next = new_term;
            new_term->prev = last_term;
            new_term->next = nullptr;
            last_term = new_term;
        }
    }
    if (new_polynomial == nullptr) {
        new_polynomial = new polynomial;
        new_term->coefficient = 0;
        new_polynomial->first_term = new_term;
    }
    return new_polynomial;
}

void MainWindow::UpdateBase(QString &input, bool show_message) {
    if (input == "") input = "0";
    polynomial* new_base_element = MakeNewPolynomial(input);
    QString new_polynomial = PolynomialToString(new_base_element);
    if (show_message) MessageResult(new_polynomial);

    if (ui->checkBox->isChecked()) {
        if (new_base_element == 0) {
            term *tmp = new term;
            tmp->coefficient = 0;
            new_base_element = new polynomial;
            new_base_element->first_term = tmp;
        }
        if (base == NULL) {
            base = new_base_element;
            last = base;
        } else {
            last->next = new_base_element;
            new_base_element->prev = last;
            last = last->next;
        }
        counter_polynomials++;
        ui->tableWidget_base->setRowCount(counter_polynomials);
        QTableWidgetItem* itm = new QTableWidgetItem();
        itm->setFlags(itm->flags() ^ Qt::ItemIsEditable);
        itm->setText(new_polynomial);
        ui->tableWidget_base->setItem(counter_polynomials - 1, 0, itm);

        CancelAddition();
        Cancelsubtraction();
        CancelMultiplication();
        CancelDivision();
    } else {
        delete new_base_element;
    }
}

polynomial* FindElementInList(int num) {
    polynomial* ptr = base;
    for (int i = 0; i < num; ++i) {
        ptr = ptr->next;
    }
    return ptr;
}

void MainWindow::on_button_delete_clicked()
{
    CancelAddition();
    Cancelsubtraction();
    CancelMultiplication();
    CancelDivision();
    QTableWidgetItem* itm = ui->tableWidget_base->currentItem();

    if (itm == nullptr) return;
    int num = itm->row();

    polynomial* ptr = FindElementInList(num);

    if (ptr->prev != nullptr) {
        ptr->prev->next = ptr->next;
    } else {
        base = base->next;
    }
    if (ptr->next != nullptr) {
        ptr->next->prev = ptr->prev;
    } else {
        last = ptr->prev;
    }
    delete ptr;

    if (itm == nullptr) return;
    ui->tableWidget_base->removeRow(itm->row());
    counter_polynomials--;
}

int CountValue(polynomial* ptr, std::map <int, int> &map) {
    int result = 0;
    term* term_ptr = ptr->first_term;
    while (term_ptr != nullptr) {
        int count = term_ptr->coefficient;
        for (int i = 0; i < 26; ++i) {
             if (term_ptr->variables[i] != 0) {
                 count *= pow(map[i], term_ptr->variables[i]);
            }
        }
        result += count;
        term_ptr = term_ptr->next;
    }
    return result;
}
void MainWindow::on_button_point_clicked()
{
    CancelAddition();
    Cancelsubtraction();
    CancelMultiplication();
    CancelDivision();
    QTableWidgetItem* itm = ui->tableWidget_base->currentItem();
    if (itm == nullptr) return;

    int num = itm->row();
    polynomial* ptr = FindElementInList(num);
    term* term_ptr = ptr->first_term;

    std::map <int, int> map;

    while (term_ptr != nullptr) {
        for (int i = 0; i < 26; ++i) {
            if (term_ptr->variables[i] != 0 && map.count(i) == 0) {
                bool bOk;
                QString inp = QInputDialog::getText(0, "Value ",
                    QString('a'+ i ), QLineEdit::Normal, "0", &bOk);
                if (bOk) {
                    int value = inp.toInt();
                    map[i] = value;
                }
            }
        }
       term_ptr = term_ptr->next;
    }
    int result = CountValue(ptr, map);
    QString res = QString::number(result);
    UpdateBase(res, true);
}

void MainWindow::on_butto_derivative_clicked()
{
    CancelAddition();
    Cancelsubtraction();
    CancelMultiplication();
    QTableWidgetItem* itm = ui->tableWidget_base->currentItem();
    if (itm == nullptr) return;

    bool bOk;
    QString inp = "";
    while (inp == "" || inp.size() > 1
           || inp[0] <= 'a' || inp[0] >= 'z') {
        inp = QInputDialog::getText(0, "Variable ",
                                            "a-z", QLineEdit::Normal, "", &bOk);
        if (bOk) {
            break;
        }
    }


    QString derivstive_number = QInputDialog::getText(0, "Order of differentiation",
                                        "1...100000000", QLineEdit::Normal, "1", &bOk);
    int number = derivstive_number.toInt();
    int tmp_number = number;


    int num = itm->row();
    polynomial* ptr = FindElementInList(num);
    term* term_ptr =ptr->first_term;

    int ind =  inp[0].unicode() - (int)'a';
    QString result = "";

    polynomial* new_pol;
    number = std::min(number, 1000000000);
    while (number--) {
        if (tmp_number != number+1) {
            delete new_pol;
            if (result == "") break;
            new_pol =  MakeNewPolynomial(result);
            term_ptr = new_pol->first_term;
        }
        result = "";
        while (term_ptr != nullptr) {
            int count = term_ptr->coefficient;
            if (term_ptr ->variables[ind] == 0) {
                term_ptr = term_ptr->next;
                continue;
            }
            count *= term_ptr->variables[ind];
            if (abs(count) == 1) {
                if (result == "" && count > 0) result+="1";
                else if (result == "") result += "-1";
                else  result += QString::number(count);
            }
            else {
                if (count > 0) {
                    if (result != "") result += "+";
                    result += QString::number(count);
                }  else if (count < 0) {
                    result += QString::number(count);
                }
            }
            for (int i = 0; i < 26; ++i) {
                if (i == ind) {
                    if (term_ptr ->variables[i] == 1) continue;
                    if (term_ptr ->variables[i] == 2) {
                        result += QString('a' + i);
                        continue;
                    }
                    result += QString('a' + i) + "^" + QString::number(term_ptr ->variables[i]-1);
                } else if (term_ptr ->variables[i] != 0) {
                    if (term_ptr ->variables[i] == 1) {
                        result += QString('a' + i);
                        continue;
                     }
                    result += QString('a' + i) + "^" + QString::number(term_ptr ->variables[i]);
                }
            }
            term_ptr = term_ptr->next;
        }
    }

    UpdateBase(result, true);
}

void MainWindow::on_button_combine_clicked()
{
    CancelAddition();
    Cancelsubtraction();
    CancelMultiplication();
    CancelDivision();
    QString input = ui->textEdit_add_polynomial->toPlainText();

    if (CheckPolynomial(input))
        UpdateBase(input, true);
    ui->textEdit_add_polynomial->clear();
}

void MainWindow::on_button_add_clicked()
{
    Cancelsubtraction();
    CancelMultiplication();
    CancelDivision();
    QTableWidgetItem* itm = ui->tableWidget_base->currentItem();
    if (itm == nullptr) return;
    if (turn_addition == 1) {
            first_add = itm->text();
            first_add_itm = itm;
            itm->setBackground(Qt::lightGray);
            turn_addition = 2;
    } else {
        second_add = itm->text();
        second_add_itm = itm;
        turn_addition = 1;
        itm->setBackground(Qt::lightGray);

        QString new_polynom = first_add;
        if (second_add[0] != '-') new_polynom += "+";
        new_polynom += second_add;
        UpdateBase(new_polynom, true);
    }
}

polynomial* Subtract(polynomial* first, polynomial* second) {
    QString new_polynom = PolynomialToString(first);
    QString secondstr = PolynomialToString(second);

    for (int i = 0; i < secondstr.size(); ++i) {
        if (secondstr[i] == '+') secondstr[i] = '-';
        else if (secondstr[i] == '-') secondstr[i] = '+';
    }

    if (secondstr[0] != '+') new_polynom += "-";
    new_polynom += secondstr;

    return MakeNewPolynomial(new_polynom);
}

void MainWindow::on_button_subtract_clicked()
{
    CancelAddition();
    CancelMultiplication();
    CancelDivision();
    QTableWidgetItem* itm = ui->tableWidget_base->currentItem();
    if (itm == nullptr) return;

    if (turn_subtraction == 1) {
            first_subtract  = itm->text();
            first_subtract_itm = itm;
            itm->setBackground(Qt::lightGray);
            turn_subtraction = 2;
    } else {
        second_subtract = itm->text();
        second_subtract_itm = itm;
        turn_subtraction = 1;
        itm->setBackground(Qt::lightGray);

        QString new_polynom = first_subtract;

        for (int i = 0; i < second_subtract.size(); ++i) {
            if (second_subtract[i] == '+') second_subtract[i] = '-';
            else if (second_subtract[i] == '-') second_subtract[i] = '+';
        }

        if (second_subtract[0] != '+') new_polynom += "-";
        new_polynom += second_subtract;
        UpdateBase(new_polynom, true);
    }
}

polynomial* Multiply(polynomial* first, polynomial* second) {
    term* tmp1 = first->first_term;
    term* tmp2 = second->first_term;

    polynomial* new_polynomial = new polynomial;
    term* last = new_polynomial->first_term;

    while (tmp1 != nullptr) {
        while (tmp2!= nullptr) {
            term* new_term = new term;
            new_term->coefficient = tmp1->coefficient * tmp2->coefficient;
            if (new_term ->coefficient == 0) {
                delete new_term;
                tmp2 = tmp2->next;
                continue;
            }
            for (int i = 0; i < 26; ++i) {
                new_term->variables[i] = tmp1->variables[i] + tmp2->variables[i];
            }

            new_term->prev = last;
            if (last != nullptr) last->next = new_term;
            else {
                last = new_term;
                new_polynomial ->first_term = new_term;
            }
            tmp2 = tmp2->next;
            last = new_term;
        }
        tmp1 = tmp1->next;
        tmp2 = second->first_term;
    }
    return new_polynomial;
}

void MainWindow::on_button_multiply_clicked()
{
    CancelAddition();
    Cancelsubtraction();
    CancelDivision();
    QTableWidgetItem* itm = ui->tableWidget_base->currentItem();
    if (itm == nullptr) return;

    if (turn_multiplication == 1) {
            first_multiply_itm = itm;
            itm->setBackground(Qt::lightGray);
            turn_multiplication = 2;
            first_multiply = FindElementInList(first_multiply_itm->row());
    } else {
        second_multiply_itm = itm;
        turn_multiplication = 1;
        itm->setBackground(Qt::lightGray);
        second_multiply = FindElementInList(second_multiply_itm->row());

        polynomial* new_polynomial = Multiply(first_multiply, second_multiply);
        QString result = PolynomialToString(new_polynomial);
        UpdateBase(result, true);
        delete new_polynomial;
    }
}

inline void MessageCantSolve(QString &polynomial) {
    QMessageBox::critical(0, "Error", "Can't find the roots of the equation\n" + polynomial + " = 0");
}

inline void MessageWrongEquation() {
    QMessageBox::critical(0, "Error", "It is not an equation");
}

void MainWindow::on_button_roots_clicked()
{
    CancelAddition();
    Cancelsubtraction();
    CancelMultiplication();
    QTableWidgetItem* itm = ui->tableWidget_base->currentItem();
    if (itm == nullptr) return;

    polynomial* ptr_polynomial = FindElementInList(itm->row());
    term* ptr_term = ptr_polynomial->first_term;
    int found_elem = -1, free_term = 10000, min_coef;
    while (ptr_term != nullptr) {
        bool flag = false;
        for (int i = 0; i < 26; ++i) {
            if (ptr_term ->variables[i] != 0 &&
                    ptr_term ->variables[i] < free_term) {
                flag = false;
                free_term = ptr_term ->variables[i];
                min_coef = ptr_term->coefficient;
            }
            if (ptr_term ->variables[i] != 0) {
                if (found_elem != i && found_elem != -1) {
                    QString tmp = PolynomialToString(ptr_polynomial);
                    MessageCantSolve(tmp);
                    return;
                }
                found_elem = i;
            }
        }
        if (!flag) {
            free_term = 0;
            min_coef = ptr_term ->coefficient;
        }
        ptr_term = ptr_term->next;
    }
    if (found_elem == -1) {
        MessageWrongEquation();
        return;
    }
    std::map <int,int> map;
    std::vector <int> answers;
    min_coef = abs(min_coef);
    for (int i = 0; i * i <= min_coef; ++i) {
        int value = -1;
        if (i == 0 || min_coef % i == 0) {
            map[found_elem] = i;
            value = CountValue(ptr_polynomial, map);
            if (value == 0) answers.push_back(i);

            if (i != 0) {
                map[found_elem] = -i;
                value = CountValue(ptr_polynomial, map);
                if (value == 0) answers.push_back(-i);
            }
            if (i !=0 && i * i != min_coef) {
                map[found_elem] = min_coef/i;
                value = CountValue(ptr_polynomial, map);
                if (value == 0) answers.push_back(min_coef/i);

                map[found_elem] = -min_coef/i;
                value = CountValue(ptr_polynomial, map);
                if (value == 0) answers.push_back(-min_coef/i);
            }
        }
    }

    QString result = "";
    QString elem = QString('a'+ found_elem);
    for (auto num: answers) {
        QString s = QString::number(num);
        result += elem + " = " + s + "\n";
    }
    if (result.size() == 0)
        QMessageBox::information(0, "Result", "The equation has no integer solutiond");
    else
        QMessageBox::information(0, "Result", result);
}

void MainWindow::on_button_divide_clicked()
{
    CancelAddition();
    Cancelsubtraction();
    CancelMultiplication();

    QTableWidgetItem* itm = ui->tableWidget_base->currentItem();
    if (itm == nullptr) return;

    if (turn_division == 1) {
            first_division_itm = itm;
            itm->setBackground(Qt::lightGray);
            turn_division = 2;
            first_divide = FindElementInList(first_division_itm->row());
    } else {
        second_division_itm = itm;
        turn_division = 1;
        itm->setBackground(Qt::lightGray);
        second_divide = FindElementInList(second_division_itm->row());
\
        polynomial* dividend = new polynomial;
        term* ptr = first_divide->first_term, *ptr_last = dividend->first_term;

        int var1 = -2;
        while (ptr != nullptr) {
            term* new_node = new term;
            if (ptr_last == nullptr) {
                dividend->first_term = new_node;
                ptr_last = new_node;
            } else {
                new_node->prev = ptr_last;
                ptr_last->next = new_node;
                ptr_last = new_node;
            }
            new_node->coefficient = ptr->coefficient;
            for (int i = 0; i < 26; ++i) {
                new_node->variables[i] = ptr->variables[i];
                if (new_node->variables[i] != 0) {
                    if (var1 == -2) var1 = i;
                    else if (var1 != -2 && var1 != i) {
                        var1 = -1;
                        break;
                    }
                }
            }
            ptr = ptr->next;
        }

        term* second = second_divide->first_term;
        int var2 = -2;
        while (second != nullptr) {
            for (int i = 0; i < 26; ++i) {
                if (second->variables[i] == 0) continue;
                if (var2 == -2) var2 = i;
                else if (var2 != -2 && var2 != i && var2 != var1) {
                    var2 = -1;
                    break;
                }
            }
            second = second->next;
        }

        if (var1 == -1 || var2 == -1 || var1 != var2) {
            QMessageBox::critical(0, "Error", "Can't devide a polynomial with several variables");
            CancelDivision();
            return;
        }

        polynomial* result = new polynomial;
        term* last = result->first_term;

        term* ptr2 = second_divide->first_term;

        polynomial* tmp = new polynomial;
        term* ptr_new  = new term;

        term* ptr1 = dividend->first_term;
        term* tmp_max = dividend->first_term;
        while (tmp_max != nullptr) {
            if (tmp_max->variables[var1] > ptr1->variables[var1])
                ptr1 = tmp_max;
            tmp_max = tmp_max->next;
        }

        while (ptr1!=nullptr) {
           ptr_new = new term;
           tmp->first_term = ptr_new;
           if (ptr1->coefficient % ptr2->coefficient != 0)
               break;
           if (ptr2->variables[var1] > ptr1->variables[var1])
               break; // в остаток
           ptr_new->variables[var1] = ptr1->variables[var1] - ptr2->variables[var1];
           ptr_new->coefficient = ptr1->coefficient / ptr2->coefficient;

           polynomial* y = Multiply(tmp, second_divide);
           polynomial* x = Subtract(dividend, y);
           delete dividend;
           delete y;
           dividend = x;
           term* tmp_max = dividend->first_term;
           ptr1 = dividend->first_term;
           while (tmp_max != nullptr) {
               if (tmp_max->variables[var1] > ptr1->variables[var1])
                   ptr1 = tmp_max;
               tmp_max = tmp_max->next;
           }

           if (last == nullptr) {
               result ->first_term = ptr_new;
               last = ptr_new;
           } else {
               ptr_new->prev = last;
               last->next = ptr_new;
               last = ptr_new;
           }
        }

        QString ost = PolynomialToString(dividend);

        QString res = PolynomialToString(result);

        if (dividend->first_term != nullptr) {
            QMessageBox::information(0, "Result", "Quotient " + res + "\nremainder " + ost);
        }
        UpdateBase(res, true);
        delete dividend;
        delete result;
    }

}


void MainWindow::on_button_save_clicked()
{
    if (QFile::exists("C:/praki/polynomials2/write.txt")) {
        QFile::remove("C:/praki/polynomials2/write.txt");
    }
    if (file_write.open(QIODevice::ReadWrite)) {

        QTextStream out(&file_write);
        for (int i = 0; i < counter_polynomials; ++i) {
            QString text = ui->tableWidget_base->item(i, 0)->text() + '\n';
            out << text;
        }
    }
        file_write.close();
}

void MainWindow::on_button_file_read_clicked()
{
    if (!file_read.open(QIODevice::ReadOnly)) {
        qWarning("Cannot open file for reading");
    }
    else{
        QTextStream in(&file_read);

        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line == "") continue;
            if (CheckPolynomial(line)) UpdateBase(line, false);
        }
    }
    file_read.close();
}

#pragma once
#include <QObject>

class C14TestCases : public QObject
{
    Q_OBJECT
public:
    explicit C14TestCases(QObject *parent = nullptr);

private slots:

    void test_private_is_qobject();

    void test_private_traits();

    void test_private_invoke();

    void test_private_value();

    void test_some();

    void test_map();

    void test_assign();

    void test_set();

    void test_set_should_check_nullptr();

    void test_get();

    void test_pick();

    void test_omit();

    void test_forIn();

    void spec_forIn_arg1_QObject_should_support_dynamic_properties();

    void spec_forIn_should_support_QVariant();

    void test_forEach();

    void spec_forEach_should_support_QVariant();

    void spec_forEach_should_support_registeredQtMetable();

    void test_reduce();

    void test_countBy();

    void test_isCollection();

    void test_assignWith();

};


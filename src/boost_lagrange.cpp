// g++ -std=c++11 -I`python -c 'from distutils.sysconfig import *; print(get_python_inc())'` -DPIC -shared -fPIC -o _lagrange.so boost_lagrange.cpp -lboost_python-py35
#include <memory>
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include "lagrange.hpp"


template <typename T_>
class boost_multiprecision_to_float {
public:
    typedef T_ native_type;

	static PyObject* convert(const native_type &value) {
		return boost::python::incref(boost::python::object(static_cast<long double>(value)).ptr());
	}
};


template <typename T_>
class float_converter {
public:
    typedef T_ native_type;

    static void * convertible(PyObject *pyo) {
        namespace py = boost::python;
        if ( ! py::extract<double>(py::object(py::handle<>(py::borrowed(pyo)))).check() ) {
            return nullptr;
        }

        return pyo;
    }

    static void construct(PyObject *pyo, boost::python::converter::rvalue_from_python_stage1_data *data) {
        namespace py = boost::python;
        double value = py::extract<double>(py::object(py::handle<>(py::borrowed(pyo))));
        native_type *storage = new(reinterpret_cast<py::converter::rvalue_from_python_storage<native_type>*>(data)->storage.bytes) native_type(value);
        data->convertible = storage;
    }
};


template <typename T_>
class pylist_to_vector_converter {
public:
    typedef T_ native_type;

    static void * convertible(PyObject *pyo) {
        if ( ! PySequence_Check(pyo) ) {
            return nullptr;
        }

        return pyo;
    }

    static void construct(PyObject *pyo, boost::python::converter::rvalue_from_python_stage1_data *data)
    {
        namespace py = boost::python;
        native_type *storage = new(reinterpret_cast<py::converter::rvalue_from_python_storage<native_type>*>(data)->storage.bytes) native_type();
        for (py::ssize_t i = 0, l = PySequence_Size(pyo); i < l; ++i) {
            storage->push_back(
                py::extract<typename boost::range_value<native_type>::type>(
                    PySequence_GetItem(pyo, i)));
        }
        data->convertible = storage;
    }
};


template <typename T>
void init_Lagrange(const std::string &name) {
    boost::python::class_<Lagrange<T>, std::shared_ptr<Lagrange<T>>>(name.c_str(), boost::python::init<std::vector<T>, std::vector<T>>())
        .def("__call__", &Lagrange<T>::operator())
        ;

    // T to python::float converter
    boost::python::to_python_converter<T, boost_multiprecision_to_float<T>>();

    // python::float to T converter
    boost::python::converter::registry::push_back(
        &float_converter<T>::convertible,
        &float_converter<T>::construct,
        boost::python::type_id<T>());

    // python::list to std::vector<T> converter
    boost::python::converter::registry::push_back(
        &pylist_to_vector_converter<std::vector<T>>::convertible,
        &pylist_to_vector_converter<std::vector<T>>::construct,
        boost::python::type_id<std::vector<T>>());
}


double py_fast(double x, const std::vector<double> &xs, const std::vector<double> &ws) {
    if ( xs.size() != ws.size() ) {
        PyErr_SetString(PyExc_AssertionError, "array must be same langth.");
        boost::python::throw_error_already_set();
    }
    return Lagrange<double>::fast(x, xs, ws);
}


BOOST_PYTHON_MODULE(_lagrange) {
    init_Lagrange<boost::multiprecision::cpp_dec_float_50>("Lagrange_f50");
    init_Lagrange<boost::multiprecision::cpp_dec_float_100>("Lagrange_f100");

    boost::python::def("fast", &py_fast);
    // python::list to std::vector<T> converter
    boost::python::converter::registry::push_back(
        &pylist_to_vector_converter<std::vector<double>>::convertible,
        &pylist_to_vector_converter<std::vector<double>>::construct,
        boost::python::type_id<std::vector<double>>());
}

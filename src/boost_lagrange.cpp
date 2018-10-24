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
class vector_to_pylist_converter {
public:
    typedef T_ native_type;

    static PyObject * convert(const native_type &v) {
        boost::python::list retval;
        for (auto i : v) {
            retval.append(boost::python::object(i));
        }
        return boost::python::incref(retval.ptr());
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
std::string vector_repr(const std::vector<T> &self) {
    boost::python::list pylist;
    for (auto item : self) {
        pylist.append(boost::python::object(item));
    }
    return boost::python::extract<std::string>(boost::python::str(pylist));
}


template <typename T>
inline void lagrange_input_assertion(const std::vector<T> &x, const std::vector<T> &w) {
    if ( x.size() != w.size() ) {
        PyErr_SetString(PyExc_AssertionError, "array must be same langth.");
        boost::python::throw_error_already_set();
    }
}


template <typename T>
std::shared_ptr<Lagrange<T>> make_Lagrange(const std::vector<T> &x, const std::vector<T> &w) {
    lagrange_input_assertion<T>(x, w);
    return std::make_shared<Lagrange<T>>(x, w);
}

template<typename T>
boost::python::list tolist(const Lagrange<T> &l) {
    boost::python::list r;
    for (const auto & c : l.coefficients ) {
        r.append(static_cast<long double>(c));
    }
    return r;
}

template <typename T>
void init_Lagrange(const std::string &name) {
    boost::python::class_<Lagrange<T>, std::shared_ptr<Lagrange<T>>>(("Lagrange_" + name).c_str(), boost::python::no_init)
        .def("__init__", boost::python::make_constructor(&make_Lagrange<T>))
        .def("__call__", &Lagrange<T>::operator())
        .def_readonly("c", &Lagrange<T>::coefficients)
        .def("coefficients", &tolist<T>)
        ;

    // T to python::float converter
    boost::python::to_python_converter<T, boost_multiprecision_to_float<T>>();

    // python::float to T converter
    boost::python::converter::registry::push_back(
        &float_converter<T>::convertible,
        &float_converter<T>::construct,
        boost::python::type_id<T>())
        ;

    // std::vector<T> to python::list converter
    //boost::python::to_python_converter<std::vector<T>, vector_to_pylist_converter<std::vector<T>>>();
    boost::python::class_<std::vector<T>>((name + "_vector").c_str())
        .def(boost::python::vector_indexing_suite<std::vector<T>>())
        .def("__repr__", &vector_repr<T>)
        ;

    // python::list to std::vector<T> converter
    boost::python::converter::registry::push_back(
        &pylist_to_vector_converter<std::vector<T>>::convertible,
        &pylist_to_vector_converter<std::vector<T>>::construct,
        boost::python::type_id<std::vector<T>>())
        ;
}

////////////////////////////////////////////////////////////////////////////////

template <typename T>
std::shared_ptr<DeltaLagrange<T>> make_DeltaLagrange(const std::vector<T> &x, const std::vector<T> &w) {
    lagrange_input_assertion<T>(x, w);
    return std::make_shared<DeltaLagrange<T>>(x, w);
}

template<typename T>
boost::python::tuple totuple(const DeltaLagrange<T> &l) {
    boost::python::list r;
    for (const auto & c : l.coefficients ) {
        r.append(static_cast<long double>(c));
    }
    return boost::python::make_tuple(
        r,
        static_cast<long double>(l.x0),
        static_cast<long double>(l.w0)
    );
}

template <typename T>
void init_DeltaLagrange(const std::string &name) {
    boost::python::class_<DeltaLagrange<T>, std::shared_ptr<DeltaLagrange<T>>>(("DeltaLagrange_" + name).c_str(), boost::python::no_init)
        .def("__init__", boost::python::make_constructor(&make_DeltaLagrange<T>))
        .def("__call__", &DeltaLagrange<T>::operator())
        .def_readonly("c", &DeltaLagrange<T>::coefficients)
        .def("coefficients", &totuple<T>)
        ;
}

double py_fast(double x, const std::vector<double> &xs, const std::vector<double> &ws) {
    lagrange_input_assertion<double>(xs, ws);
    return Lagrange<double>::fast(x, xs, ws);
}


BOOST_PYTHON_MODULE(_lagrange) {
    init_Lagrange<boost::multiprecision::cpp_dec_float_50>("float_50");
    init_Lagrange<boost::multiprecision::cpp_dec_float_100>("float_100");

    init_DeltaLagrange<boost::multiprecision::cpp_dec_float_50>("float_50");
    init_DeltaLagrange<boost::multiprecision::cpp_dec_float_100>("float_100");

    boost::python::def("fast", &py_fast);
    // python::list to std::vector<T> converter
    boost::python::converter::registry::push_back(
        &pylist_to_vector_converter<std::vector<double>>::convertible,
        &pylist_to_vector_converter<std::vector<double>>::construct,
        boost::python::type_id<std::vector<double>>());
}


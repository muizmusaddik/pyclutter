#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "pyclutter.h"

struct _PyClutterCallback
{
        PyObject *func;
        PyObject *data;

        gint n_params;
        GType *param_types;
};

PyClutterCallback *
pyclutter_callback_new (PyObject *func,
                        PyObject *data,
                        gint      n_params,
                        GType     param_types[])
{
        PyClutterCallback *retval;

        g_return_val_if_fail (func != NULL, NULL);

        retval = g_new0 (PyClutterCallback, 1);
        
        retval->func = func;
        Py_INCREF (retval->func);
        
        if (data) {
                retval->data = data;
                Py_INCREF (retval->data);
        }

        retval->n_params = n_params;
        if (retval->n_params) {
                if (!param_types) {
                        g_warning ("n_params is %d but param_types is "
                                   "NULL in pyclutter_callback_new()",
                                   n_params);
                        retval->n_params = 0;
                }
                else {
                        retval->param_types = g_new (GType, n_params);
                        memcpy (retval->param_types, param_types,
                                n_params * sizeof (GType));
                }
        }

        return retval;
}

void
pyclutter_callback_free (PyClutterCallback *cb)
{
        if (G_LIKELY (cb)) {
                if (cb->func) {
                        Py_DECREF (cb->func);
                        cb->func = NULL;
                }

                if (cb->data) {
                        Py_DECREF (cb->data);
                        cb->data = NULL;
                }

                if (cb->param_types) {
                        g_free (cb->param_types);
                        cb->n_params = 0;
                        cb->param_types = NULL;
                }

                g_free (cb);
        }
}

PyObject *
pyclutter_callback_invoke (PyClutterCallback *cb,
                           ...)
{
        va_list var_args;
        int i;
        PyObject *args;
        PyObject *retobj;

        if (G_UNLIKELY (!cb)) {
                g_warning ("Invalid callback set");
                return NULL;
        }

        args = PyTuple_New (cb->n_params + 1);

        va_start (var_args, cb);

        if (cb->n_params) {
                for (i = 0; i < cb->n_params; i++) {
                        PyObject *param = va_arg (var_args, PyObject*);

                        PyTuple_SetItem (args, i, param);
                }
        }
        else {
                i = 0;
        }
        
        va_end (var_args);
        
        /* append the data */
        if (cb->data) {
                PyTuple_SetItem (args, i, cb->data);
        }

        retobj = PyObject_CallObject (cb->func, args);

        Py_DECREF (args);

        return retobj;
}
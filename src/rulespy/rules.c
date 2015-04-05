#include <Python.h>
#include <rules.h>

static PyObject *RulesError;

static PyObject *pyCreateRuleset(PyObject *self, PyObject *args) {
    char *name;
    char *rules;
    unsigned int stateCacheSize;
    if (!PyArg_ParseTuple(args, "lss", &stateCacheSize, &name, &rules)) {
        PyErr_SetString(RulesError, "pyCreateRuleset Invalid argument");
        return NULL;
    }

    void *output = NULL;
    unsigned int result = createRuleset(&output, name, rules, stateCacheSize);
    if (result != RULES_OK) {
        if (result == ERR_OUT_OF_MEMORY) {
            PyErr_NoMemory();
        } else { 
            char * message;
            asprintf(&message, "Could not create ruleset, error code: %d", result);
            PyErr_SetString(RulesError, message);
            free(message);
        }
        return NULL;
    }

    return Py_BuildValue("l", output);
}

static PyObject *pyDeleteRuleset(PyObject *self, PyObject *args) {
    void *handle;
    if (!PyArg_ParseTuple(args, "l", &handle)) {
        PyErr_SetString(RulesError, "pyDeleteRuleset Invalid argument");
        return NULL;
    }

    unsigned int result = deleteRuleset(handle);
    if (result != RULES_OK) {
        if (result == ERR_OUT_OF_MEMORY) {
            PyErr_NoMemory();
        } else { 
            char * message;
            asprintf(&message, "Could not delete ruleset, error code: %d", result);
            PyErr_SetString(RulesError, message);
            free(message);
        }
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *pyBindRuleset(PyObject *self, PyObject *args) {
    void *handle;
    char *host;
    unsigned int port;
    char *password = NULL;
    unsigned int result;
    if (PyArg_ParseTuple(args, "lzls", &handle, &password, &port, &host)) {
        result = bindRuleset(handle, host, port, password);
    } else {
        PyErr_SetString(RulesError, "pyBindRuleset Invalid argument");
        return NULL;
    }

    if (result != RULES_OK) {
        if (result == ERR_OUT_OF_MEMORY) {
            PyErr_NoMemory();
        } else { 
            char * message;
            asprintf(&message, "Could not create connection, error code: %d", result);
            PyErr_SetString(RulesError, message);
            free(message);
        }
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *pyComplete(PyObject *self, PyObject *args) {
    void *rulesBinding = NULL;
    unsigned int replyCount = 0;
    unsigned int result;
    if (PyArg_ParseTuple(args, "li", &rulesBinding, &replyCount)) {
        result = complete(rulesBinding, replyCount);
    } else {
        PyErr_SetString(RulesError, "pyComplete Invalid argument");
        return NULL;
    }

    if (result != RULES_OK) {
        if (result == ERR_OUT_OF_MEMORY) {
            PyErr_NoMemory();
        } else { 
            char * message;
            asprintf(&message, "Could not complete, error code: %d", result);
            PyErr_SetString(RulesError, message);
            free(message);
        }
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *pyAssertEvent(PyObject *self, PyObject *args) {
    void *handle;
    char *event;
    if (!PyArg_ParseTuple(args, "ls", &handle, &event)) {
        PyErr_SetString(RulesError, "pyAssertEvent Invalid argument");
        return NULL;
    }

    unsigned int result = assertEvent(handle, event);
    if (result == RULES_OK) {
        return Py_BuildValue("i", 1);    
    } else if (result == ERR_EVENT_NOT_HANDLED) {
        return Py_BuildValue("i", 0);    
    } else {
        if (result == ERR_OUT_OF_MEMORY) {
            PyErr_NoMemory();
        } else { 
            char * message;
            asprintf(&message, "Could not assert event, error code: %d", result);  
            PyErr_SetString(RulesError, message);
            free(message);
        }
        return NULL;
    }
}

static PyObject *pyAssertEvents(PyObject *self, PyObject *args) {
    void *handle;
    char *events;
    if (!PyArg_ParseTuple(args, "ls", &handle, &events)) {
        PyErr_SetString(RulesError, "pyAssertEvents Invalid argument");
        return NULL;
    }

    unsigned int *results = NULL;
    unsigned int resultsLength;
    unsigned int result = assertEvents(handle, events, &resultsLength, &results);
    if (result == RULES_OK) {
        if (results) {
            free(results);
        }
        return Py_BuildValue("i", resultsLength);
    } else {
        if (result == ERR_OUT_OF_MEMORY) {
            PyErr_NoMemory();
        } else { 
            if (results) {
                free(results);
            }
            char * message;
            asprintf(&message, "Could not assert events, error code: %d", result);  
            PyErr_SetString(RulesError, message);
            free(message);
        }
        return NULL;
    }
}

static PyObject *pyRetractEvent(PyObject *self, PyObject *args) {
    void *handle;
    char *event;
    if (!PyArg_ParseTuple(args, "ls", &handle, &event)) {
        PyErr_SetString(RulesError, "pyRetractEvent Invalid argument");
        return NULL;
    }

    unsigned int result = retractEvent(handle, event);
    if (result == RULES_OK) {
        return Py_BuildValue("i", 1);    
    } else if (result == ERR_EVENT_NOT_HANDLED) {
        return Py_BuildValue("i", 0);    
    } else {
        if (result == ERR_OUT_OF_MEMORY) {
            PyErr_NoMemory();
        } else { 
            char * message;
            asprintf(&message, "Could not retract event, error code: %d", result);  
            PyErr_SetString(RulesError, message);
            free(message);
        }
        return NULL;
    }
}

static PyObject *pyStartAssertFact(PyObject *self, PyObject *args) {
    void *handle;
    char *fact;
    if (!PyArg_ParseTuple(args, "ls", &handle, &fact)) {
        PyErr_SetString(RulesError, "pyAssertFact Invalid argument");
        return NULL;
    }

    unsigned short replyCount;
    void *rulesBinding = NULL;
    unsigned int result = startAssertFact(handle, fact, &rulesBinding, &replyCount);
    if (result == RULES_OK) {
        return Py_BuildValue("li", rulesBinding, replyCount);    
    } else if (result == ERR_EVENT_NOT_HANDLED) {
        return Py_BuildValue("li", 0, 0);    
    } else {
        if (result == ERR_OUT_OF_MEMORY) {
            PyErr_NoMemory();
        } else { 
            char * message;
            asprintf(&message, "Could not assert fact, error code: %d", result);  
            PyErr_SetString(RulesError, message);
            free(message);
        }
        return NULL;
    }
}

static PyObject *pyAssertFact(PyObject *self, PyObject *args) {
    void *handle;
    char *fact;
    if (!PyArg_ParseTuple(args, "ls", &handle, &fact)) {
        PyErr_SetString(RulesError, "pyAssertFact Invalid argument");
        return NULL;
    }

    unsigned int result = assertFact(handle, fact);
    if (result == RULES_OK) {
        return Py_BuildValue("i", 1);    
    } else if (result == ERR_EVENT_NOT_HANDLED) {
        return Py_BuildValue("i", 0);    
    } else {
        if (result == ERR_OUT_OF_MEMORY) {
            PyErr_NoMemory();
        } else { 
            char * message;
            asprintf(&message, "Could not assert fact, error code: %d", result);  
            PyErr_SetString(RulesError, message);
            free(message);
        }
        return NULL;
    }
}

static PyObject *pyStartAssertFacts(PyObject *self, PyObject *args) {
    void *handle;
    char *facts;
    if (!PyArg_ParseTuple(args, "ls", &handle, &facts)) {
        PyErr_SetString(RulesError, "pyAssertFact Invalid argument");
        return NULL;
    }

    unsigned short replyCount;
    void *rulesBinding = NULL;
    unsigned int *results = NULL;
    unsigned int resultsLength;
    unsigned int result = startAssertFacts(handle, facts, &resultsLength, &results, &rulesBinding, &replyCount);
    if (result == RULES_OK) {  
        if (results) {
            free(results);
        }
        return Py_BuildValue("li", rulesBinding, replyCount);    
    } else if (result == ERR_EVENT_NOT_HANDLED) {
        return Py_BuildValue("li", 0, 0);    
    } else {
        if (result == ERR_OUT_OF_MEMORY) {
            PyErr_NoMemory();
        } else { 
            // if (results) {
            //     free(results);
            // } 
            char * message;
            asprintf(&message, "Could not assert facts, error code: %d", result);  
            PyErr_SetString(RulesError, message);
            free(message);
        }
        return NULL;
    }
}

static PyObject *pyAssertFacts(PyObject *self, PyObject *args) {
    void *handle;
    char *facts;
    if (!PyArg_ParseTuple(args, "ls", &handle, &facts)) {
        PyErr_SetString(RulesError, "pyAssertFacts Invalid argument");
        return NULL;
    }

    unsigned int *results = NULL;
    unsigned int resultsLength;
    unsigned int result = assertFacts(handle, facts, &resultsLength, &results);
    if (result == RULES_OK) {   
        if (results) {
            free(results);
        }
        return Py_BuildValue("i", resultsLength);
    } else {
        if (result == ERR_OUT_OF_MEMORY) {
            PyErr_NoMemory();
        } else {
            if (results) {
                free(results);
            } 
            char * message;
            asprintf(&message, "Could not assert facts, error code: %d", result);  
            PyErr_SetString(RulesError, message);
            free(message);
        }
        return NULL;
    }
}

static PyObject *pyRetractFact(PyObject *self, PyObject *args) {
    void *handle;
    char *fact;
    if (!PyArg_ParseTuple(args, "ls", &handle, &fact)) {
        PyErr_SetString(RulesError, "pyRetractFact Invalid argument");
        return NULL;
    }

    unsigned int result = retractFact(handle, fact);
    if (result == RULES_OK) {
        return Py_BuildValue("i", 1);    
    } else if (result == ERR_EVENT_NOT_HANDLED) {
        return Py_BuildValue("i", 0);    
    } else {
        if (result == ERR_OUT_OF_MEMORY) {
            PyErr_NoMemory();
        } else { 
            char * message;
            asprintf(&message, "Could not retract fact, error code: %d", result);  
            PyErr_SetString(RulesError, message);
            free(message);
        }
        return NULL;
    }
}

static PyObject *pyRetractFacts(PyObject *self, PyObject *args) {
    void *handle;
    char *facts;
    if (!PyArg_ParseTuple(args, "ls", &handle, &facts)) {
        PyErr_SetString(RulesError, "pyAssertFacts Invalid argument");
        return NULL;
    }

    unsigned int *results = NULL;
    unsigned int resultsLength;
    unsigned int result = retractFacts(handle, facts, &resultsLength, &results);
    if (result == RULES_OK) {   
        if (results) {
            free(results);
        }
        return Py_BuildValue("i", resultsLength);
    } else {
        if (result == ERR_OUT_OF_MEMORY) {
            PyErr_NoMemory();
        } else {
            if (results) {
                free(results);
            } 
            char * message;
            asprintf(&message, "Could not retract facts, error code: %d", result);  
            PyErr_SetString(RulesError, message);
            free(message);
        }
        return NULL;
    }
}

static PyObject *pyAssertState(PyObject *self, PyObject *args) {
    void *handle;
    char *state;
    if (!PyArg_ParseTuple(args, "ls", &handle, &state)) {
        PyErr_SetString(RulesError, "pyAssertState Invalid argument");
        return NULL;
    }

    unsigned int result = assertState(handle, state);
    if (result == RULES_OK) {
        return Py_BuildValue("i", 1);    
    } else if (result == ERR_EVENT_NOT_HANDLED) {
        return Py_BuildValue("i", 0);    
    } else {
        if (result == ERR_OUT_OF_MEMORY) {
            PyErr_NoMemory();
        } else { 
            char * message;
            asprintf(&message, "Could not assert state, error code: %d", result);
            PyErr_SetString(RulesError, message);
            free(message);
        }
        return NULL;
    }
}

static PyObject *pyStartAction(PyObject *self, PyObject *args) {
    void *handle;
    if (!PyArg_ParseTuple(args, "l", &handle)) {
        PyErr_SetString(RulesError, "pyStartAction Invalid argument");
        return NULL;
    }
    
    char *state;
    char *messages;
    void *actionHandle;
    unsigned int result = startAction(handle, &state, &messages, &actionHandle);
    if (result == ERR_NO_ACTION_AVAILABLE) {
        Py_RETURN_NONE;
    } else if (result != RULES_OK) {
        if (result == ERR_OUT_OF_MEMORY) {
            PyErr_NoMemory();
        } else { 
            char * message;
            asprintf(&message, "Could not start action, error code: %d", result);
            PyErr_SetString(RulesError, message);
            free(message);
        }
        return NULL;
    }

    PyObject *returnValue = Py_BuildValue("ssl", state, messages, actionHandle);
    return returnValue;
}

static PyObject *pyCompleteAction(PyObject *self, PyObject *args) {
    void *handle;
    void *actionHandle;
    char *state;
    if (!PyArg_ParseTuple(args, "lls", &handle, &actionHandle, &state)) {
        PyErr_SetString(RulesError, "pyCompleteAction Invalid argument");
        return NULL;
    }

    unsigned int result = completeAction(handle, actionHandle, state);
    if (result != RULES_OK) {
        if (result == ERR_OUT_OF_MEMORY) {
            PyErr_NoMemory();
        } else { 
            char * message;
            asprintf(&message, "Could not complete action, error code: %d", result);
            PyErr_SetString(RulesError, message);
            free(message);
        }
        return NULL;
    }

    Py_RETURN_NONE;  
}

static PyObject *pyAbandonAction(PyObject *self, PyObject *args) {
    void *handle;
    void *actionHandle;
    if (!PyArg_ParseTuple(args, "ll", &handle, &actionHandle)) {
        PyErr_SetString(RulesError, "pyAbandonAction Invalid argument");
        return NULL;
    }

    unsigned int result = abandonAction(handle, actionHandle);
    if (result != RULES_OK) {
        if (result == ERR_OUT_OF_MEMORY) {
            PyErr_NoMemory();
        } else { 
            char * message;
            asprintf(&message, "Could not abandon action, error code: %d", result);
            PyErr_SetString(RulesError, message);
            free(message);
        }
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *pyStartTimer(PyObject *self, PyObject *args) {
    void *handle;
    char *sid;
    int duration = 0;
    char *timer = NULL;
    if (!PyArg_ParseTuple(args, "lsis", &handle, &sid, &duration, &timer)) {
        PyErr_SetString(RulesError, "pyStartTimer Invalid argument");
        return NULL;
    }

    unsigned int result = startTimer(handle, sid, duration, timer);
    if (result != RULES_OK) {
        if (result == ERR_OUT_OF_MEMORY) {
            PyErr_NoMemory();
        } else { 
            char * message;
            asprintf(&message, "Could not start timer, error code: %d", result);
            PyErr_SetString(RulesError, message);
            free(message);
        }
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *pyAssertTimers(PyObject *self, PyObject *args) {
    void *handle;
    if (!PyArg_ParseTuple(args, "l", &handle)) {
        PyErr_SetString(RulesError, "pyAssertTimers Invalid argument");
        return NULL;
    }

    unsigned int result = assertTimers(handle);
    if (result == RULES_OK) {
        return Py_BuildValue("i", 1);    
    } else if (result == ERR_NO_TIMERS_AVAILABLE) {
        return Py_BuildValue("i", 0);    
    } else {
        if (result == ERR_OUT_OF_MEMORY) {
            PyErr_NoMemory();
        } else { 
            char * message;
            asprintf(&message, "Could not assert timers, error code: %d", result);
            PyErr_SetString(RulesError, message);
            free(message);
        }
        return NULL;
    }
}

static PyObject *pyGetState(PyObject *self, PyObject *args) {
    void *handle;
    char *sid;
    if (!PyArg_ParseTuple(args, "ls", &handle, &sid)) {
        PyErr_SetString(RulesError, "pyGetState Invalid argument");
        return NULL;
    }

    char *state;
    unsigned int result = getState(handle, sid, &state);
    if (result != RULES_OK) {
        if (result == ERR_OUT_OF_MEMORY) {
            PyErr_NoMemory();
        } else { 
            char * message;
            asprintf(&message, "Could not get state, error code: %d", result);
            PyErr_SetString(RulesError, message);
            free(message);
        }
        return NULL;
    }
    PyObject *returnValue = Py_BuildValue("s", state);
    free(state);
    return returnValue;
}

static PyMethodDef myModule_methods[] = {
    {"create_ruleset", pyCreateRuleset, METH_VARARGS},
    {"delete_ruleset", pyDeleteRuleset, METH_VARARGS},
    {"bind_ruleset", pyBindRuleset, METH_VARARGS},
    {"complete", pyComplete, METH_VARARGS},
    {"assert_event", pyAssertEvent, METH_VARARGS},
    {"assert_events", pyAssertEvents, METH_VARARGS},
    {"retract_event", pyRetractEvent, METH_VARARGS},
    {"start_assert_fact", pyStartAssertFact, METH_VARARGS},
    {"assert_fact", pyAssertFact, METH_VARARGS},
    {"start_assert_facts", pyStartAssertFacts, METH_VARARGS},
    {"assert_facts", pyAssertFacts, METH_VARARGS},
    {"retract_fact", pyRetractFact, METH_VARARGS},
    {"retract_facts", pyRetractFacts, METH_VARARGS},
    {"assert_state", pyAssertState, METH_VARARGS},
    {"start_action", pyStartAction, METH_VARARGS},
    {"complete_action", pyCompleteAction, METH_VARARGS},
    {"abandon_action", pyAbandonAction, METH_VARARGS},
    {"start_timer", pyStartTimer, METH_VARARGS},
    {"assert_timers", pyAssertTimers, METH_VARARGS},
    {"get_state", pyGetState, METH_VARARGS},
    {NULL, NULL}
};

PyMODINIT_FUNC initrules()
{
    PyObject *m = Py_InitModule("rules", myModule_methods);
    if (m != NULL) {
        RulesError = PyErr_NewException("rules.error", NULL, NULL);
        Py_INCREF(RulesError);
        PyModule_AddObject(m, "error", RulesError);
    }
}
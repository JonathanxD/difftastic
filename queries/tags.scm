; Modules
(module) @name @reference.module
(import alias: (identifier) @name) @reference.module
(remote_type_identifier
  module: (identifier) @name) @reference.module
((field_access
  record: (identifier) @name)
 (#is-not? local)) @reference.module

; Functions
(function
  name: (identifier) @name) @definition.function
(public_function
  name: (identifier) @name) @definition.function
(external_function
  name: (identifier) @name) @definition.function
(public_external_function
  name: (identifier) @name) @definition.function
(unqualified_import (identifier) @name) @reference.function
((function_call
   function: (identifier) @name) @reference.function
 (#is-not? local))
((field_access
  record: (identifier) @ignore
  field: (label) @name)
 (#is-not? local)) @reference.function
((binary_expression
   operator: "|>"
   right: (identifier) @name)
 (#is-not? local)) @reference.function

; Types
(public_type_definition
  (type_name
    name: (type_identifier) @name)) @definition.type
(type_definition
  (type_name
    name: (type_identifier) @name)) @definition.type
(public_type_definition
  (type_constructors
    (type_constructor
      name: (type_identifier) @name))) @definition.type
(type_definition
  (type_constructors
    (type_constructor
      name: (type_identifier) @name))) @definition.type
(external_type
  (type_name
    name: (type_identifier) @name)) @definition.type
(public_external_type
  (type_name
    name: (type_identifier) @name)) @definition.type

(type_identifier) @name @reference.type

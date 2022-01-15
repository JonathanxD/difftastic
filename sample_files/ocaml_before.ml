type foo =
  | Bar
  | Baz of int * ('ex, 'en) list
  | Biz of ('ex, 'en) list

let do_stuff x =
  match x with
  | `Foo -> 1
  | `Bar -> 2
  | _ -> 3

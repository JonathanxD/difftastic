// SIP-44
class C:
// ^keyword
  fooooo.map: x =>
  // ^type
  //     ^method.call
    x + 1

  xs.map:
    param1 =>
      param1 + 1

  foooo:
  // ^function.call
    println("")

  foooo `++`:
  //    ^operator
    val x = 1
    List(x)

  // This is an ascription
  val y = x: Int
  //         ^type

  // This is SIP-44
  val y = x:
    Int
    //^constant

// Ascription expression
class C:
  foooo: Int
  //     ^type

enum Test(a: Int) derives Codec:
// ^keyword   
//            ^type
//                   ^keyword
//                          ^type
//    ^type.definition
//        ^parameter
  case Test(b: String)
  // ^keyword     
  //               ^type
  //      ^type.definition
  //        ^parameter
  case Hello, Bla
  //      ^type.definition
  //          ^type.definition
  case Bla extends Test(256)
  //          ^keyword

opaque type Blow <: Int = 25
// ^type.qualifier
//      ^keyword     
//                   ^type     
//            ^type.definition

inline given Test = new Test {
// ^ storageclass
  inline def hello(inline x: Boolean) = 
// ^ storageclass
//                   ^ storageclass
    inline if x then "hi" else "bye" 
    // ^storageclass
    //            ^conditional
    inline x match 
    // ^storageclass
      case true => 25 
      case false => 26 
}

object A:
// ^ keyword
//     ^type.definition

  ::(123)
//^function.call
//   ^number

object bla:
  open class Hello(A: Int)
// ^ type.qualifier
  transparent trait Hello
//   ^ type.qualifier
  infix def hello = 25
//  ^ keyword

extension (s: String)
//  ^keyword
  def test = 25
  def test2 = 25

val extension = "hello"
//     ^variable - to test "soft" keywords

given Test with
// ^keyword
//     ^type
//          ^keyword
   def test = 1
//  ^keyword.function
   val a = "hello"


class Copier:
  private val printUnit = new Printer { type PrinterType = InkJet }
  private val scanUnit = new Scanner

  export scanUnit.scan
  // ^ include
  //        ^namespace
  export printUnit.{status as _, *}
  // ^ include
  //        ^namespace

  def status: List[String] = printUnit.status ++ scanUnit.status

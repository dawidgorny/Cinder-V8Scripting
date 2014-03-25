
var obj = new TestObject(3);
print("obj created");
var obj2 = new TestObject(2);
print("obj2 created");

obj.plusOne();
print("obj plusOne");

var obj3 = new ExampleObject();
print("obj3 created");
print("obj3.getValue() : " + obj3.getValue() );
obj3.setValue(7);
print("obj3.getValue() : " + obj3.getValue() );

function ProcessLine() {

}

function update()
{
	//print("test update");

}

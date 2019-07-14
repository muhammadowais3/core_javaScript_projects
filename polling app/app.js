


var emailRef = document.getElementById('email');
var passwordRef = document.getElementById('password');
var usernameRef = document.getElementById('username');
var phoneRef = document.getElementById('phone');
var errorRef = document.getElementById('error');

function signup() {
	console.log('signup invoke', emailRef.value, passwordRef.value, usernameRef.value, phoneRef.value);
    firebase.auth().createUserWithEmailAndPassword(emailRef.value, passwordRef.value)
	.then((success) => {
		console.log('signup successfully', success);
		location = './login.html';
	})
	.catch((error) => {
		console.error('something went wrong', error);
		errorRef.innerHTML = error.message;
	})
}

function login() {
	console.log('login invoke', emailRef.value, passwordRef.value);
    firebase.auth().signInWithEmailAndPassword(emailRef.value, passwordRef.value)
	.then((success) => {
		console.log('signin successfully', success.user);
		localStorage.setItem('currentUserUid', success.user.uid)
		location = './home.html';
	})
	.catch((error) => {
		console.log('something went wrong', error)
	})
}

var input  = document.createElement('input');
var input1 = document.createElement('input');
var input2 = document.createElement('input');
var input3 = document.createElement('input');
var input4 = document.createElement('input');
var button = document.createElement('button');
var buttonText = document.createTextNode('Conclude your poll');
var br = document.createElement("br");

input.setAttribute('type','text')
input.setAttribute('placeholder','Enter Your Question')
input.setAttribute('id','question');

input1.setAttribute('type','text')
input1.setAttribute('placeholder','Enter Your option 1')
input1.setAttribute('id','option1');

input2.setAttribute('type','text')
input2.setAttribute('placeholder','Enter Your option 2')
input2.setAttribute('id','option2');

input3.setAttribute('type','text')
input3.setAttribute('placeholder','Enter Your option 3')
input3.setAttribute('id','option3');

input4.setAttribute('type','text')
input4.setAttribute('placeholder','Enter Your name')
input4.setAttribute('id','name');

button.setAttribute('onclick','concludePoll()');
button.appendChild(buttonText)
var divRef = document.getElementById('divv');



function addPoll(){
	// console.log('todoObject');
	
	divRef.appendChild(input4);
	divRef.appendChild(input);
	divRef.appendChild(input1);
	divRef.appendChild(input2);
	divRef.appendChild(input3);
	divRef.appendChild(button);
	divRef.removeChild(document.getElementById("addPole"));
	
}
 






function concludePoll() {
	var quesRef = document.getElementById('question');
	var op1Ref =  document.getElementById('option1');
	var op2Ref =  document.getElementById('option2');
	var op3Ref =  document.getElementById('option3');
	var nameRef = document.getElementById('name');
	// console.log('hello', quesRef.value, op1Ref.value, op2Ref.value);
	
    var pollObject = {
			name: nameRef.value,
		    ques: quesRef.value,
		    opt1: op1Ref.value,
		    opt2: op2Ref.value,
		    opt3: op3Ref.value,
		}
    // console.log(pollObject);
    //set is used to override and set the current data
    //push is used to add new data and keep the privous one.

	let currentUserUid = firebase.auth().currentUser.uid;  
    console.log(currentUserUid);
	
    firebase.database().ref('/polls/'+currentUserUid).push(pollObject);
	
	home1();


}


var btn1 = document.createElement('button');
var butText1 = document.createTextNode('option1 vote me');
var butText2 = document.createTextNode('option2 vote me');
var butText3 = document.createTextNode('option3 vote me');


btn1.appendChild(butText1);
btn1.setAttribute("onClick","countUpbtn1()");

var btn2 = document.createElement('button');
btn2.appendChild(butText2);
btn2.setAttribute("onClick","countUpbtn2()");


var btn3 = document.createElement('button');
btn3.appendChild(butText3);
btn3.setAttribute("onClick","countUpbtn3()");
 
var p = document.createElement('p');
var li = document.createElement('li');
var resultButton = document.createElement("button");
var resultText = document.createTextNode("Result your pole");
resultButton.appendChild(resultText);
resultButton.setAttribute("onclick","result()")
var allData;

function home1(){

	fetchData();

	var textNodeQuestion = document.createTextNode(allData.ques);
	var textNodeoption1 = document.createTextNode(allData.opt1);
	var textNodeoption2 = document.createTextNode(allData.opt2);
	var textNodeoption3 = document.createTextNode(allData.opt3);
	p.appendChild(textNodeQuestion);
	divRef.appendChild(p);

	li.appendChild(textNodeoption1);
	li.appendChild(btn1);
	divRef.appendChild(li);

	li.appendChild(textNodeoption2);
	li.appendChild(btn2);
	divRef.appendChild(li);

	li.appendChild(textNodeoption3);
	li.appendChild(btn3);
	divRef.appendChild(li);


	divRef.removeChild(input4);
	divRef.removeChild(input);
	divRef.removeChild(input1);
	divRef.removeChild(input2);
	divRef.removeChild(input3);
 	divRef.removeChild(button);
 	divRef.appendChild(resultButton);


}


function fetchData() {
    // Reactive
    // Non Reactive
    let currentUserUid = localStorage.getItem('currentUserUid');
    firebase.database().ref('/polls/'+currentUserUid).on('child_added', (todoSnapshot) => {

        allData = todoSnapshot.val();
		
   
		
	})
	// console.log(allData.opt1);

}



var btn1Count=0;
var btn2Count=0;
var btn3Count=0;
// var newLi= document.createElement("li");
function countUpbtn1(){

		console.log(btn1Count++);
		// firebase.database().ref('/polls/'+currentUserUid).set(btn1Count);

	
	}
	
	
	function countUpbtn2(){
		console.log(btn2Count++)
		// firebase.database().ref('/polls/'+currentUserUid).set(btn2Count);


		
	}
	
	
	function countUpbtn3(){
		
		console.log(btn3Count++)
		// firebase.database().ref('/polls/'+currentUserUid).set(btn2Count);

	}

function result(){
	var total = btn1Count + btn2Count + btn3Count;
	var percentBtn1 = (btn1Count/total)*100;
	var percentBtn2 = (btn2Count/total)*100;
	var percentBtn3 = (btn3Count/total)*100;


	document.write("Option1 got"+percentBtn1+"Option2 got"+percentBtn2+"Option3 got"+percentBtn3)
}
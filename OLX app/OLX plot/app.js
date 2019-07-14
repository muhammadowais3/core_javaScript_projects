var emailRef = document.getElementById('email1');
var passwordRef = document.getElementById('password1');
var usernameRef = document.getElementById('username1');
var phoneRef = document.getElementById('phone1');
var errorRef = document.getElementById('error');
var loginEmailRef = document.getElementById('email');
var loginPasswordRef = document.getElementById('password');


var productNameRef =  document.getElementById('product-name');
var productDescriptionRef =  document.getElementById('product-description');
var productModelRef =  document.getElementById('product-model');
var productCatagoryRef =  document.getElementById('product-catagory');
var productImageRef =  document.getElementById('product-image');





function signup() {
    console.log('signup invoke', emailRef.value, passwordRef.value, usernameRef.value, phoneRef.value);

    // console.log(emailRef.value,'===========')

    firebase.auth().createUserWithEmailAndPassword(emailRef.value, passwordRef.value)
        .then((success) => {
            console.log('signup successfully', success);
            // location = './login.html';
            emailRef.value = "";
            passwordRef.value = "";
            usernameRef.value = "";
            phoneRef.value = "";
        })
        .catch((error) => {
            console.error('something went wrong', error.message);
            errorRef.innerHTML = error.message;
        })
}

function signout(){
    firebase.auth().signOut().then(function() {
        console.log('Signed Out');
      }, function(error) {
        console.error('Sign Out Error', error);
      });
}

function login() {
    console.log('login invoke', loginEmailRef.value, loginPasswordRef.value);
    firebase.auth().signInWithEmailAndPassword(loginEmailRef.value, loginPasswordRef.value)
        .then((success) => {
            console.log('signin successfully', success.user);
            localStorage.setItem('currentUserUid', success.user.uid)
            // location = './home.html';
            loginEmailRef.value = "";
            loginPasswordRef.value = "";
        })
        .catch((error) => {
            console.log('something went wrong', error)
        })
}


function checkUser(){
    firebase.auth().onAuthStateChanged(function(user) {
        if (user) {
          return true;
        } else {
          return false;
        }
      });
}
if(checkUser()){
    // document.getElementById("accountDetailModal").style.display = "none";
    // document.getElementById("accountDetailModaltoggle").style.display = "block"; 
    var x = checkUser();
    console.log(x);
}




function adSubmit(){
    var productDetails = {
        productName : productNameRef.value,
        productDescription : productDescriptionRef.value,
        productModel:productModelRef.value,
        productCatagory:productCatagoryRef.value,
        productImage:productImageRef.value
    }
    console.log(productDetails);
    let currentUserUid = firebase.auth().currentUser.uid;  
    console.log(currentUserUid);
    firebase.database().ref('/Data/'+ currentUserUid).push(productDetails);
    uploadImage();
}


function uploadImage(){
   
}



//chat starts
var mainBody = document.getElementById("container");
var messageRef = document.getElementById("myMessage");

function sendMessage(){
    firebase.database().ref('users/').set({
        // Uid : cuurentUID,
        message : messageRef.value
      }, function(error) {
        if (error) {
          console.log(error)
        } else {
          console.log(" Data saved successfully!"+  messageRef.value)
        }
      });
    console.log("i am working")
    }
// chat ends
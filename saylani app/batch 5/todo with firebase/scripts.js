var database = firebase.database().ref('/')
var getInput = document.getElementById('demo')
var parent = document.getElementById('parent')

function submitFunc() {
    var item = getInput.value;
    var user = {
        name: item
    }
    if (item !== "") {
        database.child('todo').push(user);
        getInput.value = "";
        getInput.focus();
    } else {
        alert("You have to Write Something")
    }
    // database.child('todo').on("child_added",function(snap){
    //     var info = snap.val()
    //     info.key = snap.key
    //     console.log(info)
    // })
}
database.child('todo').on("child_added", function(snapshot) {
    var obj = snapshot.val();
    obj.id = snapshot.key;
    console.log(obj)
    render(obj)
})

function render(data) {
    var li = document.createElement("li")
    var liText = document.createTextNode(data.name)
    li.appendChild(liText)
    li.setAttribute("class", "list-group-item list-group-item-action");
    li.setAttribute("id", data.id);

    var deleteBtn = document.createElement("BUTTON");
    var btnText = document.createTextNode("Delete");
    deleteBtn.appendChild(btnText);
    deleteBtn.setAttribute("class", "btn btn-danger float-right");
    deleteBtn.onclick = function() {
        remove(data.id);
    }

    li.appendChild(deleteBtn);
    parent.appendChild(li)

}

function remove(key) {
    database.child("todo/" + key).remove();
}
database.child("todo").on("child_removed", function(data) {
    var deletedList = document.getElementById(data.key);
    deletedList.remove();
})
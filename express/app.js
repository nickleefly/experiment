// Start Express
var express = require("express");
var app = express();
 
// Set the view directory to /views
app.set("views", __dirname + "/views");
 
// Let's use the Jade templating language
app.set("view engine", "jade");
 
// Do you love anime?
app.get("/", function(request, response) {
  response.render("index", { message: "I love anime" });
});
 
// Start that server
app.listen(1337);
var mongoose = require('mongoose');
    Schema   = mongoose.Schema;

var PostSchema = new Schema({
  title: String,
  body: String,
  date: {type: Date, default: Date.now()},
  state: {type: String, enum: ['draft', 'published', 'private'], default: 'draft'}
});

mongoose.connect('mongodb://localhost/test');
mongoose.model('Post', PostSchema);

var Post = mongoose.model('Post');

var post = new Post();
post.title = 'My first blog post';
post.body = 'Post body';
post.state = 'published';
//post.date = Date.now();

post.save(function(err) {
  if (err) {throw err;}
  console.log('saved');
  mongoose.disconnect();
})
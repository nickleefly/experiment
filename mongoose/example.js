var mongoose = require('mongoose');
    Schema   = mongoose.Schema;

var CommentSchema = new Schema({
  email: String,
  body: String
})

var PostSchema = new Schema({
  title: String,
  body: String,
  date: {type: Date, default: Date.now()},
  state: {type: String, enum: ['draft', 'published', 'private'], default: 'draft'},
  author: {
    name: String,
    email: {type: String, validate: /^[A-Za-z0-9._+\-\']+@[A-Za-z0-9.\-]+\.[A-Za-z]{2,}$/i}
  },
  comments: [CommentSchema]
});

PostSchema.static('recent', function(days, callback) {
  days = days || 1;
  this.find({date: {$gte: Date.now() - 1000 * 60 * 60 * 24 * days}}, callback);
});

mongoose.connect('mongodb://localhost/test');
mongoose.model('Post', PostSchema);

var Post = mongoose.model('Post');

var post = new Post();
post.title = 'My first blog post';
post.body = 'Post body';
post.state = 'published';
//post.date = Date.now();
post.author = {name: 'Nick', email: 'nickleefly@hotmail.com'};
post.comments.push({email:'a@b.com', body: 'beep boop'});

post.save(function(err) {
  if (err) {throw err;}
  console.log('saved');
  console.log('going to find');
  Post.recent(10, function(err, posts) {
    if (err) { throw err;}
    console.log('found');
    posts.forEach(function(data) {
      console.log(data);
      mongoose.disconnect();
    });
  });
})
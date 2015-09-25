# mruby-r3
R3 router binding

## install by mrbgems

- add conf.gem line to `build_config.rb`

```ruby
MRuby::Build.new do |conf|
    conf.gem :github => 'rail44/mruby-r3'
end
```

## example
```ruby
tree = R3::Tree.new(3) # with capacity
Method = R3::Method

tree.insert_route(Method::GET, "/hoge", {hoge: 'hogehoge'})
tree.insert_route(Method::POST, "/fuga/{i}", "5678")
tree.compile

match1 = tree.match(Method::GET, "/hoge")
p match1[:params]
#=> []
p match1[:data]
#=> {:hoge=>"hogehoge"}

match2 = tree.match(Method::POST, "/fuga/1234")
p match2[:params]
#=> ["1234"]
p match2[:data]
#=> "5678"

match3 = tree.match(Method::POST, "/hoge")
p match3[:params]
#=> []
p match3[:data]
#=> nil
```

## License
under the MIT License:
- see LICENSE file

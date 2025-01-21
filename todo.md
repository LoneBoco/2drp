# TODO

### Design



### Assets

- Develop an actual login package.
- New character sprites and tiles.
- Bomb image.

### Features

- Keybindings.
- Animated tiles.
	- Different tile render method?  Will probably require shaders.  Maybe animated tiles are the only ones rendered via shader.

### Networking

- Create a player object that stores flags, has a player number, and is networked.
- Send players on login.
- Ownership needs to be changed. The client might receive a scene object with an owner it doesn't recognize.
	- This should be better now, but might need to be reconsidered.  There is a hack on Server::GetPlayerById right now.
- Ability to swap packages for when a player connects to a server.
    - Currently created some code to let the client re-create the server.  Might need to do the same for the game's scripting and UI stuff.
- Send important files to the client, like UI files.
- Have the client request files it does not have, like font files, and not break while waiting.
- Check on file downloads.
- Improve networked physics.
	- When we change a scene object's velocity, it causes an X/Y change too, which causes a jump on other clients when you start moving.
	- Might need to come up with another solution on changing X/Y positions while using velocity.  Using it for prediction?  Interpolation?

### Optimization

- One physics body per chunk.
- Gani idle animation is constantly "dirty" so it causes constant network traffic.

### Maintenance

- Investigate Cap'n Proto for a protobuf replacement.
- Make a better distinction between server functions that send packets and those which force changes to the server state.
	- This is so host servers can use functions when getting packets.  Give and Remove vs Set?  Send vs Set?
- Prevent double calls to Physics.AddSceneObject.  Check if already added somehow.
- Confirm Variant item type works via scripting.

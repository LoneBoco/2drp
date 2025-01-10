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

- Ownership needs to be changed. The client might receive a scene object with an owner it doesn't recognize.
- Player ids identified by the network need rework for co-op games.
- Need a way to get player data, including player number.
	- Create a player object that stores flags, has a player number, and is networked.
	- Player information is sent on login.  Can be configured to be sent all at once or as needed.
- Ability to swap packages for when a player connects to a server.
- Send important files to the client, like UI files.
- Have the client request files it does not have, like font files, and not break while waiting.
- Improve networked physics.
- Check on file downloads.

### Optimization

- One physics body per chunk.

### Maintenance

- Investigate Cap'n Proto for a protobuf replacement.
- Make a better distinction between server functions that send packets and those which force changes to the server state.
	- This is so host servers can use functions when getting packets.  Give and Remove vs Set?  Send vs Set?
- Prevent double calls to Physics.AddSceneObject.  Check if already added somehow.
- Confirm Variant item type works via scripting.

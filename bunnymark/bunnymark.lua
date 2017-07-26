local M = {}


local bunnies = {}
local frame_count = 0
local start_timestamp = 0

local TEXT = "Bunnies: %d FPS: %d. Click to add more."

local draw_text_message = {
	position = vmath.vector3(10, 20, 0)
}

local BUNNY_IMAGES = {
	hash("rabbitv3_batman"),
	hash("rabbitv3_bb8"),
	hash("rabbitv3"),
	hash("rabbitv3_ash"),
	hash("rabbitv3_frankenstein"),
	hash("rabbitv3_neo"),
	hash("rabbitv3_sonic"),
	hash("rabbitv3_spidey"),
	hash("rabbitv3_stormtrooper"),
	hash("rabbitv3_superman"),
	hash("rabbitv3_tron"),
	hash("rabbitv3_wolverine"),
}

function M.start()
	M.bunnies = {}
	frame_count = 0
	start_timestamp = socket.gettime()
end

function M.stop()
	while #M.bunnies > 0 do
		local bunny = table.remove(M.bunnies)
		go.delete(bunny.id)
	end
end

function M.create_bunny()
	local id = factory.create("#factory")
	msg.post(msg.url(nil, id, "sprite"), "play_animation", { id = BUNNY_IMAGES[math.random(1, #BUNNY_IMAGES)] })
	local bunny = { id = id }
	M.bunnies[#M.bunnies + 1] = bunny
	return bunny
end

function M.update()
	frame_count = frame_count + 1
	local fps = frame_count / (socket.gettime() - start_timestamp)
	draw_text_message.text = TEXT:format(#M.bunnies, fps)
	msg.post("@render:", "draw_text", draw_text_message)
end

return M
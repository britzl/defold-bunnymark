local M = {}


local bunnies = {}
local frames = {}

local TEXT = "Bunnies: %d FPS: %.2f. Click to add more."

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

local MAX_BUNNIES = 16384

function M.start()
	M.bunnies = {}
	frames = {}
end

function M.stop()
	while #M.bunnies > 0 do
		local bunny = table.remove(M.bunnies)
		go.delete(bunny.id)
	end
end

function M.create_bunny()
	if #M.bunnies < MAX_BUNNIES-1 then
		local id = factory.create("#factory")
		msg.post(msg.url(nil, id, "sprite"), "play_animation", { id = BUNNY_IMAGES[math.random(1, #BUNNY_IMAGES)] })
		local bunny = { id = id }
		M.bunnies[#M.bunnies + 1] = bunny
		return bunny
	else
		return nil, "Max bunnies (" .. MAX_BUNNIES .. ") reached"
	end
end

function M.update()
	table.insert(frames, socket.gettime())
	local fps = 0
	if #frames == 61 then
		table.remove(frames, 1)
		fps = 1 / ((frames[#frames] - frames[1]) / (#frames - 1))
	end
	label.set_text("#label", TEXT:format(#M.bunnies, fps))
end

return M
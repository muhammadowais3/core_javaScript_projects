function hit_point(current_location, speed, direction){
    
}

// speed finder takes time @every 200ms by default
// old_loc, new_loc is obejct with x,y coord for old and new location respectively
class Ballistic{
    constructor(old_loc, new_loc, dt = 0.2){
        this.old_loc_x = old_loc.x;
        this.old_loc_y = old_loc.y;

        this.new_loc_x = new_loc.x;
        this.new_loc_y = new_loc.y;

        this.dt = dt
        
    }
    speed_finder() {
        let speed_x = (this.old_loc_x - this.new_loc_x) / this.dt;
        let speed_y = (this.old_loc_y - this.new_loc_y) / this.dt;
        this.speed = {
            x: speed_x,
            y: speed_y
        }
        // console.log("i am speed finder")
        return this.speed;
    }
    angle_finder(){
        this.speed_finder()
        let old_angle = Math.atan(this.old_loc_y / this.old_loc_x)
        let new_angle = Math.atan(this.new_loc_y / this.new_loc_x)
        this.angles = { 
            old_angle: old_angle,
            new_angle: new_angle
        }
        console.log(`old angle = ${old_angle} new angle =  ${new_angle}`)
        return this.angles;

    }
    dist_finder(){
        let old_dis_x = Math.pow((this.old_loc_x) , 2)
        let old_dis_y = Math.pow((this.old_loc_y) , 2)
        let old_dis = Math.sqrt(old_dis_x + old_dis_y)
        // console.log(this.old_loc_x , old_dis_x )
        let new_dis_x = Math.pow((this.old_loc_x) , 2)
        let new_dis_y = Math.pow((this.old_loc_y) , 2)
        let new_dis = Math.sqrt(new_dis_x + new_dis_y)
        this.dist = {
            old_dis: old_dis,
            new_dis: new_dis
        }
    }
}


let obj_ballistic_angles = []
let obj_ballistic_distances= []
let old_loc= {
    x: 35.0,
    y: 18.0
}

let new_loc= {
    x: 0.0,
    y: 18.0
}


for (let i = 0; i < 25; i++) {
    
    obj_ballistic_angles.push(new Ballistic(old_loc ,new_loc)
    .angle_finder())
    obj_ballistic_distances.push(new Ballistic(old_loc ,new_loc)
    .dist_finder())
    let diff_x = new_loc.x - old_loc.x
    let diff_y = new_loc.y - old_loc.y
    old_loc = new_loc
    new_loc = {
        x: old_loc.x + diff_x,
        y: old_loc.y + diff_y
    }
    console.log(new_loc)
}


console.log(obj_ballistic_angles, obj_ballistic_distances)
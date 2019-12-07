main
    shared rest_count := read_integer()
    shared rest_capacity [res_count] := 0[rest_count]
    queue_rest[rest_count]
    cont_rest_capacity[rest_count] := [rest_count]
    mutex := semaphore(1)

for local index := 0 to rest_cout do
        rest_capacity[index] := read_integer()

    local id := 0
    while true do
        case read_char() of
            'P': create_thread(patient(id++))
            'I': create_thread(impatient(id++))
            EOF: return

patient(id):
    wait(mutex)
    cont_rest_capacity -= 1
    signal(mutex)
    wait(queue_rest[id])
    eat()
    wait(mutex)
    cont_rest_capacity += 1
    signal(mutex)
    signal(queue_rest[id])


impatient(id):
    min_queue := 0
    min_rest := 0
    first_id := id
    valid := true
    while(valid:) 
        if rest_capacity[id] - cont_rest_capacity[id] < 0 then:
            if min_queue = 0 then:
                min_queue := cont_rest_capacity[id]
                min_rest := id  
            else if min_queue > cont_rest_capacity[id]
                min_queue := cont_rest_capacity[id]
                min_rest := id
            signal(mutex)
            id := (id+1) mod rest_count
            walk()
        else
            cont_rest_capacity -= 1
            signal(mutex)
            wait(queue_rest[id])
            eat()
            wait(mutex)
            cont_rest_capacity += 1
            signal(mutex)
            signal(queue_rest[id])
            valid := false
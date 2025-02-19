import trick


for item in vars(stl_so.stl_ip_test.__class__).items():
    item_size = 0
    
    try:
        cast_item = trick.castAsSTL_ARRAY_char
        item_size = item.size()
        print(f"Size is {item_size}")
    except Exception as e:
        print(f"Exception: {e}")

    

        

trick.stop(10)
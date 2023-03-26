


def run(input_path, model_path):
    import time
    start_time = time.time()

    torch.backends.cudnn.enabled = True
    # torch.backends.cudnn.benchmark = True
    # #

    start(input_path, "./output", model_path)
    print("--- %s seconds it took ---" % (time.time() - start_time))

    return 1


first_execution = True
def process(device, model, model_type, image, input_size, target_size, optimize, use_camera):
    """
    Run the inference and interpolate.

    Args:
        device (torch.device): the torch device used
        model: the model used for inference
        model_type: the type of the model
        image: the image fed into the neural network
        input_size: the size (width, height) of the neural network input (for OpenVINO)
        target_size: the size (width, height) the neural network output is interpolated to
        optimize: optimize the model to half-floats on CUDA?
        use_camera: is the camera used?

    Returns:
        the prediction
    """
    global first_execution

    if "openvino" in model_type:
        if first_execution or not use_camera:
            print(f"    Input resized to {input_size[0]}x{input_size[1]} before entering the encoder")
            first_execution = False

        sample = [np.reshape(image, (1, 3, *input_size))]
        prediction = model(sample)[model.output(0)][0]
        prediction = cv2.resize(prediction, dsize=target_size,
                                interpolation=cv2.INTER_CUBIC)
    else:
        sample = torch.from_numpy(image).to(device).unsqueeze(0)

        if optimize and device == torch.device("cuda"):
            if first_execution:
                print("  Optimization to half-floats activated. Use with caution, because models like Swin require/n"
                      "  float precision to work properly and may yield non-finite depth values to some extent for/n"
                      "  half-floats.")
            sample = sample.to(memory_format=torch.channels_last)
            sample = sample.half()

        if first_execution or not use_camera:
            height, width = sample.shape[2:]
            print(f"    Input resized to {width}x{height} before entering the encoder")
            first_execution = False

        prediction = model.forward(sample)
        prediction = (
            torch.nn.functional.interpolate(
                prediction.unsqueeze(1),
                size=target_size[::-1],
                mode="bicubic",
                align_corners=False,
            )
            .squeeze()
            .cpu()
            .numpy()
        )

    return prediction


def start(input_path, output_path, model_path, model_type="dpt_beit_large_512", optimize=False, side=False, height=None,
          square=False, grayscale=False):


    # create output folder
    if output_path is not None:
        os.makedirs(output_path, exist_ok=True)

    print("Start processing")
    print("  Processing {} ".format(input_path))

    # input
    original_image_rgb = utils.read_image(input_path)  # in [0, 1]
    image = transform({"image": original_image_rgb})["image"]

    # compute
    with torch.no_grad():
        prediction = process(device, model, model_type, image, (net_w, net_h), original_image_rgb.shape[1::-1],
                             optimize, False)

    # output
    filename = os.path.join(
        output_path, os.path.splitext(os.path.basename(input_path))[0] + '-' + model_type
    )
    print(filename)
    utils.write_depth(filename, prediction, grayscale, bits=2)



# Program entry point from CPP
if len(sys.argv) != 2:
    sys.exit("Not enough args")

ca_one = str(sys.argv[0])
ca_two = str(sys.argv[1])
print("My command line args are " + ca_one + " and " + ca_two)


# ca_one = "C:/Users/Flourek/CLionProjects/Stereorizer/img/hg.jpg"
# ca_two = "C:/Users/Flourek/CLionProjects/Stereorizer/weights/dpt_beit_large_512.pt"

run(ca_one, ca_two)

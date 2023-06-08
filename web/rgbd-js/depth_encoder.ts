import { NativeByteArray } from './capi_containers';
import { EmscriptenModuleEx } from './emscripten_module_ex';
import { NativeObject } from './native_object';

export class NativeDepthEncoder extends NativeObject {
  constructor(module: EmscriptenModuleEx,
              depthCodecType: number,
              width: number,
              height: number) {
    const ptr = module.ccall('rgbd_depth_encoder_ctor',
                             'number',
                             ['number', 'number', 'number'],
                             [depthCodecType, width, height]);
    super(module, ptr, true);
  }

  delete() {
    this.getModule().ccall('rgbd_depth_encoder_dtor', null, ['number'], [this.getPtr()]);
  }

  encode(depthValues: Int32Array, keyframe: boolean) {
    const depthValuesPtr = this.getModule()._malloc(depthValues.byteLength);
    // Have to do >> 2 to the pointer since the set() function interprets its second parameter
    // as an index, not a pointer.
    // https://github.com/emscripten-core/emscripten/issues/4003
    this.getModule().HEAP32.set(depthValues, depthValuesPtr >> 2);
    const byteArrayPtr = this.getModule().ccall('rgbd_depth_encoder_encode',
                                           'number',
                                           ['number', 'number', 'boolean'],
                                           [this.getPtr(), depthValuesPtr, keyframe]);
    this.getModule()._free(depthValuesPtr);

    const byteArray = new NativeByteArray(this.getModule(), byteArrayPtr);
    const bytes = byteArray.toArray();
    byteArray.close();
    return bytes;
  }
}

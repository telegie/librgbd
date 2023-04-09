import { NativeByteArray } from './capi_containers';
import { NativeObject } from './native_object';

export class NativeDepthEncoder extends NativeObject {
  constructor(wasmModule: any,
              depthCodecType: number,
              width: number,
              height: number) {
    const ptr = wasmModule.ccall('rgbd_depth_encoder_ctor',
                                     'number',
                                     ['number', 'number', 'number'],
                                     [depthCodecType, width, height]);
    super(wasmModule, ptr, true);
  }

  delete() {
    this.wasmModule.ccall('rgbd_depth_encoder_dtor', null, ['number'], [this.ptr]);
  }

  encode(depthValues: Int32Array, keyframe: boolean) {
    const depthValuesPtr = this.wasmModule._malloc(depthValues.byteLength);
    // Have to do >> 2 to the pointer since the set() function interprets its second parameter
    // as an index, not a pointer.
    // https://github.com/emscripten-core/emscripten/issues/4003
    this.wasmModule.HEAP32.set(depthValues, depthValuesPtr >> 2);
    const byteArrayPtr = this.wasmModule.ccall('rgbd_depth_encoder_encode',
                                               'number',
                                               ['number', 'number', 'boolean'],
                                               [this.ptr, depthValuesPtr, keyframe]);
    this.wasmModule._free(depthValuesPtr);

    const byteArray = new NativeByteArray(this.wasmModule, byteArrayPtr);
    const bytes = byteArray.toArray();
    byteArray.close();
    return bytes;
  }
}

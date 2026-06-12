; ModuleID = 'ptoas.hivm.official.vector'
source_filename = "ptoas.hivm.official.vector"

declare i32 @llvm.hivm.get.TID.X()

declare void @llvm.hivm.sync.workitems()

declare i64 @llvm.hivm.GET.BLOCK.IDX()

declare void @llvm.hivm.MOV.OUT.TO.UB.ALIGN.V2.f32.DV(ptr addrspace(6), ptr addrspace(1), i64, i64)

declare void @llvm.hivm.store.vfsimt.info(i64)

declare void @llvm.hivm.MOV.UB.TO.OUT.ALIGN.V2.DV(ptr addrspace(1), ptr addrspace(6), i64, i64)

; Function Attrs: noinline
define linkonce_odr simt_entry void @simt_vf_0(ptr addrspace(6) %0, ptr addrspace(1) %1, ptr addrspace(1) %2, ptr addrspace(1) %3) #0 !annotation !6 !annotation !7 {
  %5 = call i32 @llvm.hivm.get.TID.X()
  %6 = alloca float, i32 8, align 4
  %7 = alloca float, i32 8, align 4
  %8 = alloca float, i32 2, align 4
  %9 = alloca float, i32 2, align 4
  %10 = alloca float, i32 8, align 4
  %11 = alloca float, i32 8, align 4
  %12 = alloca float, i32 8, align 4
  %13 = alloca float, i32 8, align 4
  %14 = alloca float, i32 2, align 4
  br label %15

15:                                               ; preds = %18, %4
  %16 = phi i64 [ %32, %18 ], [ 0, %4 ]
  %17 = icmp slt i64 %16, 2
  br i1 %17, label %18, label %33

18:                                               ; preds = %15
  %19 = trunc i64 %16 to i32
  %20 = mul i32 %19, 1024
  %21 = mul i32 %5, 4
  %22 = add i32 %20, %21
  %23 = sext i32 %22 to i64
  %24 = ptrtoint ptr addrspace(6) %0 to i64
  %25 = inttoptr i64 %24 to ptr addrspace(6)
  %26 = mul i64 %23, 4
  %27 = getelementptr i8, ptr addrspace(6) %25, i64 %26
  %28 = load <4 x float>, ptr addrspace(6) %27, align 16
  %29 = mul i32 %19, 4
  %30 = sext i32 %29 to i64
  %31 = getelementptr float, ptr %6, i64 %30
  store <4 x float> %28, ptr %31, align 16
  %32 = add i64 %16, 1
  br label %15

33:                                               ; preds = %15
  br label %34

34:                                               ; preds = %37, %33
  %35 = phi i64 [ %52, %37 ], [ 0, %33 ]
  %36 = icmp slt i64 %35, 2
  br i1 %36, label %37, label %53

37:                                               ; preds = %34
  %38 = trunc i64 %35 to i32
  %39 = mul i32 %38, 1024
  %40 = mul i32 %5, 4
  %41 = add i32 %39, %40
  %42 = add i32 %41, 2048
  %43 = sext i32 %42 to i64
  %44 = ptrtoint ptr addrspace(6) %0 to i64
  %45 = inttoptr i64 %44 to ptr addrspace(6)
  %46 = mul i64 %43, 4
  %47 = getelementptr i8, ptr addrspace(6) %45, i64 %46
  %48 = load <4 x float>, ptr addrspace(6) %47, align 16
  %49 = mul i32 %38, 4
  %50 = sext i32 %49 to i64
  %51 = getelementptr float, ptr %7, i64 %50
  store <4 x float> %48, ptr %51, align 16
  %52 = add i64 %35, 1
  br label %34

53:                                               ; preds = %34
  br label %54

54:                                               ; preds = %90, %53
  %55 = phi i64 [ %92, %90 ], [ 0, %53 ]
  %56 = icmp slt i64 %55, 2
  br i1 %56, label %57, label %93

57:                                               ; preds = %54
  %58 = trunc i64 %55 to i32
  %59 = getelementptr float, ptr %8, i64 %55
  store float 0.000000e+00, ptr %59, align 4
  br label %60

60:                                               ; preds = %63, %57
  %61 = phi i64 [ %73, %63 ], [ 0, %57 ]
  %62 = icmp slt i64 %61, 4
  br i1 %62, label %63, label %74

63:                                               ; preds = %60
  %64 = trunc i64 %61 to i32
  %65 = load float, ptr %59, align 4
  %66 = mul i32 %58, 4
  %67 = add i32 %66, %64
  %68 = sext i32 %67 to i64
  %69 = getelementptr float, ptr %7, i64 %68
  %70 = load float, ptr %69, align 4
  %71 = fcmp ogt float %65, %70
  %72 = select i1 %71, float %65, float %70
  store float %72, ptr %59, align 4
  %73 = add i64 %61, 1
  br label %60

74:                                               ; preds = %60
  %75 = load float, ptr %59, align 4
  %76 = zext i32 %5 to i64
  %77 = getelementptr float, ptr addrspace(6) null, i64 %76
  store float %75, ptr addrspace(6) %77, align 4
  call void @llvm.hivm.sync.workitems()
  %78 = icmp eq i32 %5, 0
  br i1 %78, label %79, label %90

79:                                               ; preds = %74
  br label %80

80:                                               ; preds = %84, %79
  %81 = phi i64 [ %88, %84 ], [ 0, %79 ]
  %82 = phi float [ %87, %84 ], [ 0.000000e+00, %79 ]
  %83 = icmp slt i64 %81, 2
  br i1 %83, label %84, label %89

84:                                               ; preds = %80
  %85 = getelementptr float, ptr addrspace(6) null, i64 %81
  %86 = load float, ptr addrspace(6) %85, align 4
  %87 = fadd float %82, %86
  %88 = add i64 %81, 1
  br label %80

89:                                               ; preds = %80
  store float %82, ptr addrspace(6) null, align 4
  br label %90

90:                                               ; preds = %89, %74
  call void @llvm.hivm.sync.workitems()
  %91 = load float, ptr addrspace(6) null, align 4
  store float %91, ptr %59, align 4
  %92 = add i64 %55, 1
  br label %54

93:                                               ; preds = %54
  br label %94

94:                                               ; preds = %97, %93
  %95 = phi i64 [ %107, %97 ], [ 0, %93 ]
  %96 = icmp slt i64 %95, 8
  br i1 %96, label %97, label %108

97:                                               ; preds = %94
  %98 = trunc i64 %95 to i32
  %99 = getelementptr float, ptr %7, i64 %95
  %100 = load float, ptr %99, align 4
  %101 = ashr i32 %98, 2
  %102 = sext i32 %101 to i64
  %103 = getelementptr float, ptr %8, i64 %102
  %104 = load float, ptr %103, align 4
  %105 = fsub float %100, %104
  %106 = call float @llvm.exp.f32(float %105)
  store float %106, ptr %99, align 4
  %107 = add i64 %95, 1
  br label %94

108:                                              ; preds = %94
  br label %109

109:                                              ; preds = %144, %108
  %110 = phi i64 [ %146, %144 ], [ 0, %108 ]
  %111 = icmp slt i64 %110, 2
  br i1 %111, label %112, label %147

112:                                              ; preds = %109
  %113 = trunc i64 %110 to i32
  %114 = getelementptr float, ptr %9, i64 %110
  store float 0.000000e+00, ptr %114, align 4
  br label %115

115:                                              ; preds = %118, %112
  %116 = phi i64 [ %127, %118 ], [ 0, %112 ]
  %117 = icmp slt i64 %116, 4
  br i1 %117, label %118, label %128

118:                                              ; preds = %115
  %119 = trunc i64 %116 to i32
  %120 = load float, ptr %114, align 4
  %121 = mul i32 %113, 4
  %122 = add i32 %121, %119
  %123 = sext i32 %122 to i64
  %124 = getelementptr float, ptr %7, i64 %123
  %125 = load float, ptr %124, align 4
  %126 = fadd float %120, %125
  store float %126, ptr %114, align 4
  %127 = add i64 %116, 1
  br label %115

128:                                              ; preds = %115
  %129 = load float, ptr %114, align 4
  %130 = zext i32 %5 to i64
  %131 = getelementptr float, ptr addrspace(6) null, i64 %130
  store float %129, ptr addrspace(6) %131, align 4
  call void @llvm.hivm.sync.workitems()
  %132 = icmp eq i32 %5, 0
  br i1 %132, label %133, label %144

133:                                              ; preds = %128
  br label %134

134:                                              ; preds = %138, %133
  %135 = phi i64 [ %142, %138 ], [ 0, %133 ]
  %136 = phi float [ %141, %138 ], [ 0.000000e+00, %133 ]
  %137 = icmp slt i64 %135, 2
  br i1 %137, label %138, label %143

138:                                              ; preds = %134
  %139 = getelementptr float, ptr addrspace(6) null, i64 %135
  %140 = load float, ptr addrspace(6) %139, align 4
  %141 = fadd float %136, %140
  %142 = add i64 %135, 1
  br label %134

143:                                              ; preds = %134
  store float %136, ptr addrspace(6) null, align 4
  br label %144

144:                                              ; preds = %143, %128
  call void @llvm.hivm.sync.workitems()
  %145 = load float, ptr addrspace(6) null, align 4
  store float %145, ptr %114, align 4
  %146 = add i64 %110, 1
  br label %109

147:                                              ; preds = %109
  br label %148

148:                                              ; preds = %151, %147
  %149 = phi i64 [ %160, %151 ], [ 0, %147 ]
  %150 = icmp slt i64 %149, 8
  br i1 %150, label %151, label %161

151:                                              ; preds = %148
  %152 = trunc i64 %149 to i32
  %153 = getelementptr float, ptr %7, i64 %149
  %154 = load float, ptr %153, align 4
  %155 = ashr i32 %152, 2
  %156 = sext i32 %155 to i64
  %157 = getelementptr float, ptr %9, i64 %156
  %158 = load float, ptr %157, align 4
  %159 = fdiv float %154, %158
  store float %159, ptr %153, align 4
  %160 = add i64 %149, 1
  br label %148

161:                                              ; preds = %148
  br label %162

162:                                              ; preds = %165, %161
  %163 = phi i64 [ %180, %165 ], [ 0, %161 ]
  %164 = icmp slt i64 %163, 2
  br i1 %164, label %165, label %181

165:                                              ; preds = %162
  %166 = trunc i64 %163 to i32
  %167 = mul i32 %166, 4
  %168 = sext i32 %167 to i64
  %169 = getelementptr float, ptr %7, i64 %168
  %170 = load <4 x float>, ptr %169, align 16
  %171 = mul i32 %166, 1024
  %172 = mul i32 %5, 4
  %173 = add i32 %171, %172
  %174 = add i32 %173, 4096
  %175 = sext i32 %174 to i64
  %176 = ptrtoint ptr addrspace(6) %0 to i64
  %177 = inttoptr i64 %176 to ptr addrspace(6)
  %178 = mul i64 %175, 4
  %179 = getelementptr i8, ptr addrspace(6) %177, i64 %178
  store <4 x float> %170, ptr addrspace(6) %179, align 16
  %180 = add i64 %163, 1
  br label %162

181:                                              ; preds = %162
  br label %182

182:                                              ; preds = %185, %181
  %183 = phi i64 [ %189, %185 ], [ 0, %181 ]
  %184 = icmp slt i64 %183, 8
  br i1 %184, label %185, label %190

185:                                              ; preds = %182
  %186 = getelementptr float, ptr %7, i64 %183
  %187 = load float, ptr %186, align 4
  %188 = fadd float %187, 0x3F847AE140000000
  store float %188, ptr %186, align 4
  %189 = add i64 %183, 1
  br label %182

190:                                              ; preds = %182
  br label %191

191:                                              ; preds = %194, %190
  %192 = phi i64 [ %209, %194 ], [ 0, %190 ]
  %193 = icmp slt i64 %192, 2
  br i1 %193, label %194, label %210

194:                                              ; preds = %191
  %195 = trunc i64 %192 to i32
  %196 = mul i32 %195, 4
  %197 = sext i32 %196 to i64
  %198 = getelementptr float, ptr %7, i64 %197
  %199 = load <4 x float>, ptr %198, align 16
  %200 = mul i32 %195, 1024
  %201 = mul i32 %5, 4
  %202 = add i32 %200, %201
  %203 = add i32 %202, 6144
  %204 = sext i32 %203 to i64
  %205 = ptrtoint ptr addrspace(6) %0 to i64
  %206 = inttoptr i64 %205 to ptr addrspace(6)
  %207 = mul i64 %204, 4
  %208 = getelementptr i8, ptr addrspace(6) %206, i64 %207
  store <4 x float> %199, ptr addrspace(6) %208, align 16
  %209 = add i64 %192, 1
  br label %191

210:                                              ; preds = %191
  br label %211

211:                                              ; preds = %235, %210
  %212 = phi i64 [ %237, %235 ], [ 0, %210 ]
  %213 = icmp slt i64 %212, 8
  br i1 %213, label %214, label %238

214:                                              ; preds = %211
  %215 = getelementptr float, ptr %10, i64 %212
  store float 0.000000e+00, ptr %215, align 4
  %216 = load float, ptr %215, align 4
  %217 = getelementptr float, ptr %7, i64 %212
  %218 = load float, ptr %217, align 4
  %219 = fadd float %216, %218
  store float %219, ptr %215, align 4
  %220 = load float, ptr %215, align 4
  %221 = zext i32 %5 to i64
  %222 = getelementptr float, ptr addrspace(6) null, i64 %221
  store float %220, ptr addrspace(6) %222, align 4
  call void @llvm.hivm.sync.workitems()
  %223 = icmp eq i32 %5, 0
  br i1 %223, label %224, label %235

224:                                              ; preds = %214
  br label %225

225:                                              ; preds = %229, %224
  %226 = phi i64 [ %233, %229 ], [ 0, %224 ]
  %227 = phi float [ %232, %229 ], [ 0.000000e+00, %224 ]
  %228 = icmp slt i64 %226, 16
  br i1 %228, label %229, label %234

229:                                              ; preds = %225
  %230 = getelementptr float, ptr addrspace(6) null, i64 %226
  %231 = load float, ptr addrspace(6) %230, align 4
  %232 = fadd float %227, %231
  %233 = add i64 %226, 1
  br label %225

234:                                              ; preds = %225
  store float %227, ptr addrspace(6) null, align 4
  br label %235

235:                                              ; preds = %234, %214
  call void @llvm.hivm.sync.workitems()
  %236 = load float, ptr addrspace(6) null, align 4
  store float %236, ptr %215, align 4
  %237 = add i64 %212, 1
  br label %211

238:                                              ; preds = %211
  %239 = and i32 %5, 15
  %240 = ashr i32 %239, 1
  %241 = icmp eq i32 %240, 0
  br i1 %241, label %242, label %267

242:                                              ; preds = %238
  br label %243

243:                                              ; preds = %246, %242
  %244 = phi i64 [ %265, %246 ], [ 0, %242 ]
  %245 = icmp slt i64 %244, 2
  br i1 %245, label %246, label %266

246:                                              ; preds = %243
  %247 = trunc i64 %244 to i32
  %248 = mul i32 %247, 4
  %249 = sext i32 %248 to i64
  %250 = getelementptr float, ptr %10, i64 %249
  %251 = load <4 x float>, ptr %250, align 16
  %252 = mul i32 %247, 128
  %253 = ashr i32 %5, 4
  %254 = mul i32 %253, 8
  %255 = add i32 %252, %254
  %256 = and i32 %5, 1
  %257 = mul i32 %256, 4
  %258 = add i32 %255, %257
  %259 = add i32 %258, 12544
  %260 = sext i32 %259 to i64
  %261 = ptrtoint ptr addrspace(6) %0 to i64
  %262 = inttoptr i64 %261 to ptr addrspace(6)
  %263 = mul i64 %260, 4
  %264 = getelementptr i8, ptr addrspace(6) %262, i64 %263
  store <4 x float> %251, ptr addrspace(6) %264, align 16
  %265 = add i64 %244, 1
  br label %243

266:                                              ; preds = %243
  br label %267

267:                                              ; preds = %266, %238
  br label %268

268:                                              ; preds = %271, %267
  %269 = phi i64 [ %278, %271 ], [ 0, %267 ]
  %270 = icmp slt i64 %269, 8
  br i1 %270, label %271, label %279

271:                                              ; preds = %268
  %272 = getelementptr float, ptr %7, i64 %269
  %273 = load float, ptr %272, align 4
  %274 = getelementptr float, ptr %10, i64 %269
  %275 = load float, ptr %274, align 4
  %276 = fadd float %275, 0x3F847AE140000000
  %277 = fdiv float %273, %276
  store float %277, ptr %272, align 4
  %278 = add i64 %269, 1
  br label %268

279:                                              ; preds = %268
  br label %280

280:                                              ; preds = %315, %279
  %281 = phi i64 [ %317, %315 ], [ 0, %279 ]
  %282 = icmp slt i64 %281, 2
  br i1 %282, label %283, label %318

283:                                              ; preds = %280
  %284 = trunc i64 %281 to i32
  %285 = getelementptr float, ptr %9, i64 %281
  store float 0.000000e+00, ptr %285, align 4
  br label %286

286:                                              ; preds = %289, %283
  %287 = phi i64 [ %298, %289 ], [ 0, %283 ]
  %288 = icmp slt i64 %287, 4
  br i1 %288, label %289, label %299

289:                                              ; preds = %286
  %290 = trunc i64 %287 to i32
  %291 = load float, ptr %285, align 4
  %292 = mul i32 %284, 4
  %293 = add i32 %292, %290
  %294 = sext i32 %293 to i64
  %295 = getelementptr float, ptr %7, i64 %294
  %296 = load float, ptr %295, align 4
  %297 = fadd float %291, %296
  store float %297, ptr %285, align 4
  %298 = add i64 %287, 1
  br label %286

299:                                              ; preds = %286
  %300 = load float, ptr %285, align 4
  %301 = zext i32 %5 to i64
  %302 = getelementptr float, ptr addrspace(6) null, i64 %301
  store float %300, ptr addrspace(6) %302, align 4
  call void @llvm.hivm.sync.workitems()
  %303 = icmp eq i32 %5, 0
  br i1 %303, label %304, label %315

304:                                              ; preds = %299
  br label %305

305:                                              ; preds = %309, %304
  %306 = phi i64 [ %313, %309 ], [ 0, %304 ]
  %307 = phi float [ %312, %309 ], [ 0.000000e+00, %304 ]
  %308 = icmp slt i64 %306, 2
  br i1 %308, label %309, label %314

309:                                              ; preds = %305
  %310 = getelementptr float, ptr addrspace(6) null, i64 %306
  %311 = load float, ptr addrspace(6) %310, align 4
  %312 = fadd float %307, %311
  %313 = add i64 %306, 1
  br label %305

314:                                              ; preds = %305
  store float %307, ptr addrspace(6) null, align 4
  br label %315

315:                                              ; preds = %314, %299
  call void @llvm.hivm.sync.workitems()
  %316 = load float, ptr addrspace(6) null, align 4
  store float %316, ptr %285, align 4
  %317 = add i64 %281, 1
  br label %280

318:                                              ; preds = %280
  %319 = srem i32 %5, 2
  %320 = icmp eq i32 %319, 0
  br i1 %320, label %321, label %337

321:                                              ; preds = %318
  br label %322

322:                                              ; preds = %325, %321
  %323 = phi i64 [ %335, %325 ], [ 0, %321 ]
  %324 = icmp slt i64 %323, 2
  br i1 %324, label %325, label %336

325:                                              ; preds = %322
  %326 = trunc i64 %323 to i32
  %327 = getelementptr float, ptr %9, i64 %323
  %328 = load float, ptr %327, align 4
  %329 = mul i32 %326, 128
  %330 = ashr i32 %5, 1
  %331 = add i32 %329, %330
  %332 = add i32 %331, 12800
  %333 = sext i32 %332 to i64
  %334 = getelementptr float, ptr addrspace(6) %0, i64 %333
  store float %328, ptr addrspace(6) %334, align 4
  %335 = add i64 %323, 1
  br label %322

336:                                              ; preds = %322
  br label %337

337:                                              ; preds = %336, %318
  br label %338

338:                                              ; preds = %341, %337
  %339 = phi i64 [ %356, %341 ], [ 0, %337 ]
  %340 = icmp slt i64 %339, 2
  br i1 %340, label %341, label %357

341:                                              ; preds = %338
  %342 = trunc i64 %339 to i32
  %343 = mul i32 %342, 4
  %344 = sext i32 %343 to i64
  %345 = getelementptr float, ptr %7, i64 %344
  %346 = load <4 x float>, ptr %345, align 16
  %347 = mul i32 %342, 1024
  %348 = mul i32 %5, 4
  %349 = add i32 %347, %348
  %350 = add i32 %349, 8192
  %351 = sext i32 %350 to i64
  %352 = ptrtoint ptr addrspace(6) %0 to i64
  %353 = inttoptr i64 %352 to ptr addrspace(6)
  %354 = mul i64 %351, 4
  %355 = getelementptr i8, ptr addrspace(6) %353, i64 %354
  store <4 x float> %346, ptr addrspace(6) %355, align 16
  %356 = add i64 %339, 1
  br label %338

357:                                              ; preds = %338
  br label %358

358:                                              ; preds = %361, %357
  %359 = phi i64 [ %371, %361 ], [ 0, %357 ]
  %360 = icmp slt i64 %359, 8
  br i1 %360, label %361, label %372

361:                                              ; preds = %358
  %362 = trunc i64 %359 to i32
  %363 = getelementptr float, ptr %7, i64 %359
  %364 = load float, ptr %363, align 4
  %365 = ashr i32 %362, 2
  %366 = sext i32 %365 to i64
  %367 = getelementptr float, ptr %9, i64 %366
  %368 = load float, ptr %367, align 4
  %369 = fadd float %368, 0x3F847AE140000000
  %370 = fdiv float %364, %369
  store float %370, ptr %363, align 4
  %371 = add i64 %359, 1
  br label %358

372:                                              ; preds = %358
  br label %373

373:                                              ; preds = %397, %372
  %374 = phi i64 [ %399, %397 ], [ 0, %372 ]
  %375 = icmp slt i64 %374, 8
  br i1 %375, label %376, label %400

376:                                              ; preds = %373
  %377 = getelementptr float, ptr %10, i64 %374
  store float 0.000000e+00, ptr %377, align 4
  %378 = load float, ptr %377, align 4
  %379 = getelementptr float, ptr %7, i64 %374
  %380 = load float, ptr %379, align 4
  %381 = fadd float %378, %380
  store float %381, ptr %377, align 4
  %382 = load float, ptr %377, align 4
  %383 = zext i32 %5 to i64
  %384 = getelementptr float, ptr addrspace(6) null, i64 %383
  store float %382, ptr addrspace(6) %384, align 4
  call void @llvm.hivm.sync.workitems()
  %385 = icmp eq i32 %5, 0
  br i1 %385, label %386, label %397

386:                                              ; preds = %376
  br label %387

387:                                              ; preds = %391, %386
  %388 = phi i64 [ %395, %391 ], [ 0, %386 ]
  %389 = phi float [ %394, %391 ], [ 0.000000e+00, %386 ]
  %390 = icmp slt i64 %388, 16
  br i1 %390, label %391, label %396

391:                                              ; preds = %387
  %392 = getelementptr float, ptr addrspace(6) null, i64 %388
  %393 = load float, ptr addrspace(6) %392, align 4
  %394 = fadd float %389, %393
  %395 = add i64 %388, 1
  br label %387

396:                                              ; preds = %387
  store float %389, ptr addrspace(6) null, align 4
  br label %397

397:                                              ; preds = %396, %376
  call void @llvm.hivm.sync.workitems()
  %398 = load float, ptr addrspace(6) null, align 4
  store float %398, ptr %377, align 4
  %399 = add i64 %374, 1
  br label %373

400:                                              ; preds = %373
  br i1 %241, label %401, label %426

401:                                              ; preds = %400
  br label %402

402:                                              ; preds = %405, %401
  %403 = phi i64 [ %424, %405 ], [ 0, %401 ]
  %404 = icmp slt i64 %403, 2
  br i1 %404, label %405, label %425

405:                                              ; preds = %402
  %406 = trunc i64 %403 to i32
  %407 = mul i32 %406, 4
  %408 = sext i32 %407 to i64
  %409 = getelementptr float, ptr %10, i64 %408
  %410 = load <4 x float>, ptr %409, align 16
  %411 = mul i32 %406, 128
  %412 = ashr i32 %5, 4
  %413 = mul i32 %412, 8
  %414 = add i32 %411, %413
  %415 = and i32 %5, 1
  %416 = mul i32 %415, 4
  %417 = add i32 %414, %416
  %418 = add i32 %417, 13056
  %419 = sext i32 %418 to i64
  %420 = ptrtoint ptr addrspace(6) %0 to i64
  %421 = inttoptr i64 %420 to ptr addrspace(6)
  %422 = mul i64 %419, 4
  %423 = getelementptr i8, ptr addrspace(6) %421, i64 %422
  store <4 x float> %410, ptr addrspace(6) %423, align 16
  %424 = add i64 %403, 1
  br label %402

425:                                              ; preds = %402
  br label %426

426:                                              ; preds = %425, %400
  br label %427

427:                                              ; preds = %430, %426
  %428 = phi i64 [ %445, %430 ], [ 0, %426 ]
  %429 = icmp slt i64 %428, 2
  br i1 %429, label %430, label %446

430:                                              ; preds = %427
  %431 = trunc i64 %428 to i32
  %432 = mul i32 %431, 4
  %433 = sext i32 %432 to i64
  %434 = getelementptr float, ptr %7, i64 %433
  %435 = load <4 x float>, ptr %434, align 16
  %436 = mul i32 %431, 1024
  %437 = mul i32 %5, 4
  %438 = add i32 %436, %437
  %439 = add i32 %438, 10240
  %440 = sext i32 %439 to i64
  %441 = ptrtoint ptr addrspace(6) %0 to i64
  %442 = inttoptr i64 %441 to ptr addrspace(6)
  %443 = mul i64 %440, 4
  %444 = getelementptr i8, ptr addrspace(6) %442, i64 %443
  store <4 x float> %435, ptr addrspace(6) %444, align 16
  %445 = add i64 %428, 1
  br label %427

446:                                              ; preds = %427
  br label %447

447:                                              ; preds = %450, %446
  %448 = phi i64 [ %457, %450 ], [ 0, %446 ]
  %449 = icmp slt i64 %448, 8
  br i1 %449, label %450, label %458

450:                                              ; preds = %447
  %451 = getelementptr float, ptr %7, i64 %448
  %452 = load float, ptr %451, align 4
  %453 = getelementptr float, ptr %10, i64 %448
  %454 = load float, ptr %453, align 4
  %455 = fadd float %454, 0x3F847AE140000000
  %456 = fdiv float %452, %455
  store float %456, ptr %451, align 4
  %457 = add i64 %448, 1
  br label %447

458:                                              ; preds = %447
  call void @llvm.hivm.sync.workitems()
  br label %459

459:                                              ; preds = %681, %458
  %460 = phi i64 [ %682, %681 ], [ 0, %458 ]
  %461 = icmp slt i64 %460, 3
  br i1 %461, label %462, label %683

462:                                              ; preds = %459
  %463 = trunc i64 %460 to i32
  br label %464

464:                                              ; preds = %467, %462
  %465 = phi i64 [ %484, %467 ], [ 0, %462 ]
  %466 = icmp slt i64 %465, 2
  br i1 %466, label %467, label %485

467:                                              ; preds = %464
  %468 = trunc i64 %465 to i32
  %469 = mul i32 %468, 1024
  %470 = mul i32 %5, 4
  %471 = add i32 %469, %470
  %472 = add i32 %471, 10240
  %473 = mul i32 %463, 2048
  %474 = sub i32 %472, %473
  %475 = sext i32 %474 to i64
  %476 = ptrtoint ptr addrspace(6) %0 to i64
  %477 = inttoptr i64 %476 to ptr addrspace(6)
  %478 = mul i64 %475, 4
  %479 = getelementptr i8, ptr addrspace(6) %477, i64 %478
  %480 = load <4 x float>, ptr addrspace(6) %479, align 16
  %481 = mul i32 %468, 4
  %482 = sext i32 %481 to i64
  %483 = getelementptr float, ptr %11, i64 %482
  store <4 x float> %480, ptr %483, align 16
  %484 = add i64 %465, 1
  br label %464

485:                                              ; preds = %464
  %486 = srem i32 %463, 2
  %487 = icmp eq i32 %486, 0
  br i1 %487, label %488, label %582

488:                                              ; preds = %485
  br label %489

489:                                              ; preds = %492, %488
  %490 = phi i64 [ %513, %492 ], [ 0, %488 ]
  %491 = icmp slt i64 %490, 2
  br i1 %491, label %492, label %514

492:                                              ; preds = %489
  %493 = trunc i64 %490 to i32
  %494 = mul i32 %493, 128
  %495 = ashr i32 %5, 4
  %496 = mul i32 %495, 8
  %497 = add i32 %494, %496
  %498 = and i32 %5, 1
  %499 = mul i32 %498, 4
  %500 = add i32 %497, %499
  %501 = add i32 %500, 13056
  %502 = mul i32 %463, 256
  %503 = sub i32 %501, %502
  %504 = sext i32 %503 to i64
  %505 = ptrtoint ptr addrspace(6) %0 to i64
  %506 = inttoptr i64 %505 to ptr addrspace(6)
  %507 = mul i64 %504, 4
  %508 = getelementptr i8, ptr addrspace(6) %506, i64 %507
  %509 = load <4 x float>, ptr addrspace(6) %508, align 16
  %510 = mul i32 %493, 4
  %511 = sext i32 %510 to i64
  %512 = getelementptr float, ptr %10, i64 %511
  store <4 x float> %509, ptr %512, align 16
  %513 = add i64 %490, 1
  br label %489

514:                                              ; preds = %489
  br label %515

515:                                              ; preds = %518, %514
  %516 = phi i64 [ %525, %518 ], [ 0, %514 ]
  %517 = icmp slt i64 %516, 8
  br i1 %517, label %518, label %526

518:                                              ; preds = %515
  %519 = getelementptr float, ptr %6, i64 %516
  %520 = load float, ptr %519, align 4
  %521 = getelementptr float, ptr %11, i64 %516
  %522 = load float, ptr %521, align 4
  %523 = fmul float %520, %522
  %524 = getelementptr float, ptr %12, i64 %516
  store float %523, ptr %524, align 4
  %525 = add i64 %516, 1
  br label %515

526:                                              ; preds = %515
  br label %527

527:                                              ; preds = %551, %526
  %528 = phi i64 [ %553, %551 ], [ 0, %526 ]
  %529 = icmp slt i64 %528, 8
  br i1 %529, label %530, label %554

530:                                              ; preds = %527
  %531 = getelementptr float, ptr %13, i64 %528
  store float 0.000000e+00, ptr %531, align 4
  %532 = load float, ptr %531, align 4
  %533 = getelementptr float, ptr %12, i64 %528
  %534 = load float, ptr %533, align 4
  %535 = fadd float %532, %534
  store float %535, ptr %531, align 4
  %536 = load float, ptr %531, align 4
  %537 = zext i32 %5 to i64
  %538 = getelementptr float, ptr addrspace(6) null, i64 %537
  store float %536, ptr addrspace(6) %538, align 4
  call void @llvm.hivm.sync.workitems()
  %539 = icmp eq i32 %5, 0
  br i1 %539, label %540, label %551

540:                                              ; preds = %530
  br label %541

541:                                              ; preds = %545, %540
  %542 = phi i64 [ %549, %545 ], [ 0, %540 ]
  %543 = phi float [ %548, %545 ], [ 0.000000e+00, %540 ]
  %544 = icmp slt i64 %542, 16
  br i1 %544, label %545, label %550

545:                                              ; preds = %541
  %546 = getelementptr float, ptr addrspace(6) null, i64 %542
  %547 = load float, ptr addrspace(6) %546, align 4
  %548 = fadd float %543, %547
  %549 = add i64 %542, 1
  br label %541

550:                                              ; preds = %541
  store float %543, ptr addrspace(6) null, align 4
  br label %551

551:                                              ; preds = %550, %530
  call void @llvm.hivm.sync.workitems()
  %552 = load float, ptr addrspace(6) null, align 4
  store float %552, ptr %531, align 4
  %553 = add i64 %528, 1
  br label %527

554:                                              ; preds = %527
  br label %555

555:                                              ; preds = %558, %554
  %556 = phi i64 [ %565, %558 ], [ 0, %554 ]
  %557 = icmp slt i64 %556, 8
  br i1 %557, label %558, label %566

558:                                              ; preds = %555
  %559 = getelementptr float, ptr %13, i64 %556
  %560 = load float, ptr %559, align 4
  %561 = getelementptr float, ptr %10, i64 %556
  %562 = load float, ptr %561, align 4
  %563 = fadd float %562, 0x3F847AE140000000
  %564 = fdiv float %560, %563
  store float %564, ptr %559, align 4
  %565 = add i64 %556, 1
  br label %555

566:                                              ; preds = %555
  br label %567

567:                                              ; preds = %570, %566
  %568 = phi i64 [ %580, %570 ], [ 0, %566 ]
  %569 = icmp slt i64 %568, 8
  br i1 %569, label %570, label %581

570:                                              ; preds = %567
  %571 = getelementptr float, ptr %6, i64 %568
  %572 = load float, ptr %571, align 4
  %573 = getelementptr float, ptr %13, i64 %568
  %574 = load float, ptr %573, align 4
  %575 = fsub float %572, %574
  %576 = getelementptr float, ptr %10, i64 %568
  %577 = load float, ptr %576, align 4
  %578 = fadd float %577, 0x3F847AE140000000
  %579 = fdiv float %575, %578
  store float %579, ptr %571, align 4
  %580 = add i64 %568, 1
  br label %567

581:                                              ; preds = %567
  br label %681

582:                                              ; preds = %485
  br label %583

583:                                              ; preds = %586, %582
  %584 = phi i64 [ %598, %586 ], [ 0, %582 ]
  %585 = icmp slt i64 %584, 2
  br i1 %585, label %586, label %599

586:                                              ; preds = %583
  %587 = trunc i64 %584 to i32
  %588 = mul i32 %587, 128
  %589 = ashr i32 %5, 1
  %590 = add i32 %588, %589
  %591 = add i32 %590, 13056
  %592 = mul i32 %463, 256
  %593 = sub i32 %591, %592
  %594 = sext i32 %593 to i64
  %595 = getelementptr float, ptr addrspace(6) %0, i64 %594
  %596 = load float, ptr addrspace(6) %595, align 4
  %597 = getelementptr float, ptr %9, i64 %584
  store float %596, ptr %597, align 4
  %598 = add i64 %584, 1
  br label %583

599:                                              ; preds = %583
  br label %600

600:                                              ; preds = %603, %599
  %601 = phi i64 [ %610, %603 ], [ 0, %599 ]
  %602 = icmp slt i64 %601, 8
  br i1 %602, label %603, label %611

603:                                              ; preds = %600
  %604 = getelementptr float, ptr %6, i64 %601
  %605 = load float, ptr %604, align 4
  %606 = getelementptr float, ptr %11, i64 %601
  %607 = load float, ptr %606, align 4
  %608 = fmul float %605, %607
  %609 = getelementptr float, ptr %12, i64 %601
  store float %608, ptr %609, align 4
  %610 = add i64 %601, 1
  br label %600

611:                                              ; preds = %600
  br label %612

612:                                              ; preds = %647, %611
  %613 = phi i64 [ %649, %647 ], [ 0, %611 ]
  %614 = icmp slt i64 %613, 2
  br i1 %614, label %615, label %650

615:                                              ; preds = %612
  %616 = trunc i64 %613 to i32
  %617 = getelementptr float, ptr %14, i64 %613
  store float 0.000000e+00, ptr %617, align 4
  br label %618

618:                                              ; preds = %621, %615
  %619 = phi i64 [ %630, %621 ], [ 0, %615 ]
  %620 = icmp slt i64 %619, 4
  br i1 %620, label %621, label %631

621:                                              ; preds = %618
  %622 = trunc i64 %619 to i32
  %623 = load float, ptr %617, align 4
  %624 = mul i32 %616, 4
  %625 = add i32 %624, %622
  %626 = sext i32 %625 to i64
  %627 = getelementptr float, ptr %12, i64 %626
  %628 = load float, ptr %627, align 4
  %629 = fadd float %623, %628
  store float %629, ptr %617, align 4
  %630 = add i64 %619, 1
  br label %618

631:                                              ; preds = %618
  %632 = load float, ptr %617, align 4
  %633 = zext i32 %5 to i64
  %634 = getelementptr float, ptr addrspace(6) null, i64 %633
  store float %632, ptr addrspace(6) %634, align 4
  call void @llvm.hivm.sync.workitems()
  %635 = icmp eq i32 %5, 0
  br i1 %635, label %636, label %647

636:                                              ; preds = %631
  br label %637

637:                                              ; preds = %641, %636
  %638 = phi i64 [ %645, %641 ], [ 0, %636 ]
  %639 = phi float [ %644, %641 ], [ 0.000000e+00, %636 ]
  %640 = icmp slt i64 %638, 2
  br i1 %640, label %641, label %646

641:                                              ; preds = %637
  %642 = getelementptr float, ptr addrspace(6) null, i64 %638
  %643 = load float, ptr addrspace(6) %642, align 4
  %644 = fadd float %639, %643
  %645 = add i64 %638, 1
  br label %637

646:                                              ; preds = %637
  store float %639, ptr addrspace(6) null, align 4
  br label %647

647:                                              ; preds = %646, %631
  call void @llvm.hivm.sync.workitems()
  %648 = load float, ptr addrspace(6) null, align 4
  store float %648, ptr %617, align 4
  %649 = add i64 %613, 1
  br label %612

650:                                              ; preds = %612
  br label %651

651:                                              ; preds = %654, %650
  %652 = phi i64 [ %661, %654 ], [ 0, %650 ]
  %653 = icmp slt i64 %652, 2
  br i1 %653, label %654, label %662

654:                                              ; preds = %651
  %655 = getelementptr float, ptr %14, i64 %652
  %656 = load float, ptr %655, align 4
  %657 = getelementptr float, ptr %9, i64 %652
  %658 = load float, ptr %657, align 4
  %659 = fadd float %658, 0x3F847AE140000000
  %660 = fdiv float %656, %659
  store float %660, ptr %655, align 4
  %661 = add i64 %652, 1
  br label %651

662:                                              ; preds = %651
  br label %663

663:                                              ; preds = %666, %662
  %664 = phi i64 [ %679, %666 ], [ 0, %662 ]
  %665 = icmp slt i64 %664, 8
  br i1 %665, label %666, label %680

666:                                              ; preds = %663
  %667 = trunc i64 %664 to i32
  %668 = getelementptr float, ptr %6, i64 %664
  %669 = load float, ptr %668, align 4
  %670 = ashr i32 %667, 2
  %671 = sext i32 %670 to i64
  %672 = getelementptr float, ptr %14, i64 %671
  %673 = load float, ptr %672, align 4
  %674 = fsub float %669, %673
  %675 = getelementptr float, ptr %9, i64 %671
  %676 = load float, ptr %675, align 4
  %677 = fadd float %676, 0x3F847AE140000000
  %678 = fdiv float %674, %677
  store float %678, ptr %668, align 4
  %679 = add i64 %664, 1
  br label %663

680:                                              ; preds = %663
  br label %681

681:                                              ; preds = %581, %680
  %682 = add i64 %460, 1
  br label %459

683:                                              ; preds = %459
  br label %684

684:                                              ; preds = %687, %683
  %685 = phi i64 [ %702, %687 ], [ 0, %683 ]
  %686 = icmp slt i64 %685, 2
  br i1 %686, label %687, label %703

687:                                              ; preds = %684
  %688 = trunc i64 %685 to i32
  %689 = mul i32 %688, 1024
  %690 = mul i32 %5, 4
  %691 = add i32 %689, %690
  %692 = add i32 %691, 4096
  %693 = sext i32 %692 to i64
  %694 = ptrtoint ptr addrspace(6) %0 to i64
  %695 = inttoptr i64 %694 to ptr addrspace(6)
  %696 = mul i64 %693, 4
  %697 = getelementptr i8, ptr addrspace(6) %695, i64 %696
  %698 = load <4 x float>, ptr addrspace(6) %697, align 16
  %699 = mul i32 %688, 4
  %700 = sext i32 %699 to i64
  %701 = getelementptr float, ptr %11, i64 %700
  store <4 x float> %698, ptr %701, align 16
  %702 = add i64 %685, 1
  br label %684

703:                                              ; preds = %684
  br label %704

704:                                              ; preds = %707, %703
  %705 = phi i64 [ %714, %707 ], [ 0, %703 ]
  %706 = icmp slt i64 %705, 8
  br i1 %706, label %707, label %715

707:                                              ; preds = %704
  %708 = getelementptr float, ptr %6, i64 %705
  %709 = load float, ptr %708, align 4
  %710 = getelementptr float, ptr %11, i64 %705
  %711 = load float, ptr %710, align 4
  %712 = fmul float %709, %711
  %713 = getelementptr float, ptr %12, i64 %705
  store float %712, ptr %713, align 4
  %714 = add i64 %705, 1
  br label %704

715:                                              ; preds = %704
  br label %716

716:                                              ; preds = %751, %715
  %717 = phi i64 [ %753, %751 ], [ 0, %715 ]
  %718 = icmp slt i64 %717, 2
  br i1 %718, label %719, label %754

719:                                              ; preds = %716
  %720 = trunc i64 %717 to i32
  %721 = getelementptr float, ptr %9, i64 %717
  store float 0.000000e+00, ptr %721, align 4
  br label %722

722:                                              ; preds = %725, %719
  %723 = phi i64 [ %734, %725 ], [ 0, %719 ]
  %724 = icmp slt i64 %723, 4
  br i1 %724, label %725, label %735

725:                                              ; preds = %722
  %726 = trunc i64 %723 to i32
  %727 = load float, ptr %721, align 4
  %728 = mul i32 %720, 4
  %729 = add i32 %728, %726
  %730 = sext i32 %729 to i64
  %731 = getelementptr float, ptr %12, i64 %730
  %732 = load float, ptr %731, align 4
  %733 = fadd float %727, %732
  store float %733, ptr %721, align 4
  %734 = add i64 %723, 1
  br label %722

735:                                              ; preds = %722
  %736 = load float, ptr %721, align 4
  %737 = zext i32 %5 to i64
  %738 = getelementptr float, ptr addrspace(6) null, i64 %737
  store float %736, ptr addrspace(6) %738, align 4
  call void @llvm.hivm.sync.workitems()
  %739 = icmp eq i32 %5, 0
  br i1 %739, label %740, label %751

740:                                              ; preds = %735
  br label %741

741:                                              ; preds = %745, %740
  %742 = phi i64 [ %749, %745 ], [ 0, %740 ]
  %743 = phi float [ %748, %745 ], [ 0.000000e+00, %740 ]
  %744 = icmp slt i64 %742, 2
  br i1 %744, label %745, label %750

745:                                              ; preds = %741
  %746 = getelementptr float, ptr addrspace(6) null, i64 %742
  %747 = load float, ptr addrspace(6) %746, align 4
  %748 = fadd float %743, %747
  %749 = add i64 %742, 1
  br label %741

750:                                              ; preds = %741
  store float %743, ptr addrspace(6) null, align 4
  br label %751

751:                                              ; preds = %750, %735
  call void @llvm.hivm.sync.workitems()
  %752 = load float, ptr addrspace(6) null, align 4
  store float %752, ptr %721, align 4
  %753 = add i64 %717, 1
  br label %716

754:                                              ; preds = %716
  br label %755

755:                                              ; preds = %758, %754
  %756 = phi i64 [ %770, %758 ], [ 0, %754 ]
  %757 = icmp slt i64 %756, 8
  br i1 %757, label %758, label %771

758:                                              ; preds = %755
  %759 = trunc i64 %756 to i32
  %760 = getelementptr float, ptr %6, i64 %756
  %761 = load float, ptr %760, align 4
  %762 = ashr i32 %759, 2
  %763 = sext i32 %762 to i64
  %764 = getelementptr float, ptr %9, i64 %763
  %765 = load float, ptr %764, align 4
  %766 = fsub float %761, %765
  %767 = getelementptr float, ptr %11, i64 %756
  %768 = load float, ptr %767, align 4
  %769 = fmul float %766, %768
  store float %769, ptr %760, align 4
  %770 = add i64 %756, 1
  br label %755

771:                                              ; preds = %755
  br label %772

772:                                              ; preds = %775, %771
  %773 = phi i64 [ %789, %775 ], [ 0, %771 ]
  %774 = icmp slt i64 %773, 2
  br i1 %774, label %775, label %790

775:                                              ; preds = %772
  %776 = trunc i64 %773 to i32
  %777 = mul i32 %776, 4
  %778 = sext i32 %777 to i64
  %779 = getelementptr float, ptr %6, i64 %778
  %780 = load <4 x float>, ptr %779, align 16
  %781 = mul i32 %776, 1024
  %782 = mul i32 %5, 4
  %783 = add i32 %781, %782
  %784 = sext i32 %783 to i64
  %785 = ptrtoint ptr addrspace(6) %0 to i64
  %786 = inttoptr i64 %785 to ptr addrspace(6)
  %787 = mul i64 %784, 4
  %788 = getelementptr i8, ptr addrspace(6) %786, i64 %787
  store <4 x float> %780, ptr addrspace(6) %788, align 16
  %789 = add i64 %773, 1
  br label %772

790:                                              ; preds = %772
  ret void
}

define void @mhc_sinkhorn_bwd_ascend_kernel_kernel_mix_aiv(ptr addrspace(1) %0, ptr addrspace(1) %1, ptr addrspace(1) %2, i32 %3) #1 {
  %5 = call i64 @llvm.hivm.GET.BLOCK.IDX()
  %6 = trunc i64 %5 to i32
  %7 = add i32 %3, 2047
  %8 = ashr i32 %7, 11
  %9 = sext i32 %8 to i64
  br label %10

10:                                               ; preds = %13, %4
  %11 = phi i64 [ %31, %13 ], [ 0, %4 ]
  %12 = icmp slt i64 %11, %9
  br i1 %12, label %13, label %32

13:                                               ; preds = %10
  %14 = trunc i64 %11 to i32
  %15 = mul i32 %14, 64
  %16 = add i32 %15, %6
  %17 = add i32 %3, 31
  %18 = ashr i32 %17, 5
  %19 = icmp sgt i32 %18, 8
  %20 = select i1 %19, i32 8, i32 %18
  %21 = sdiv i32 %16, %20
  %22 = mul i32 %21, %20
  %23 = mul i32 %22, 2048
  %24 = srem i32 %16, %20
  %25 = mul i32 %24, 2048
  %26 = add i32 %23, %25
  %27 = sext i32 %26 to i64
  %28 = getelementptr float, ptr addrspace(1) %1, i64 %27
  call void @llvm.hivm.MOV.OUT.TO.UB.ALIGN.V2.f32.DV(ptr addrspace(6) null, ptr addrspace(1) %28, i64 274877906960, i64 0)
  %29 = getelementptr float, ptr addrspace(1) %2, i64 %27
  call void @llvm.hivm.MOV.OUT.TO.UB.ALIGN.V2.f32.DV(ptr addrspace(6) getelementptr (i8, ptr addrspace(6) null, i64 8192), ptr addrspace(1) %29, i64 274877906960, i64 0)
  call void @llvm.hivm.store.vfsimt.info(i64 4295033088)
  call simt_entry void @simt_vf_0(ptr addrspace(6) null, ptr addrspace(1) %2, ptr addrspace(1) %1, ptr addrspace(1) %0)
  %30 = getelementptr float, ptr addrspace(1) %0, i64 %27
  call void @llvm.hivm.MOV.UB.TO.OUT.ALIGN.V2.DV(ptr addrspace(1) %30, ptr addrspace(6) null, i64 274877906960, i64 9007199254749184)
  %31 = add i64 %11, 1
  br label %10

32:                                               ; preds = %10
  ret void
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare float @llvm.exp.f32(float) #2

attributes #0 = { noinline "target-cpu"="dav-c310-vec" "target-features"="+ATOMIC,+ArchV130,+AregRedefinable,+ArithmeticBf16,+AtomicForB8 ,+F8e4m3,+F8e5m2,+F8e8m0,+FFTSBlk,+Fp4e1m2x2,+Fp4e2m1x2,+LDExtRefine,+MOVX8,+MSTX,+SPR7bits,+SyncV,+dav-c310-vec" }
attributes #1 = { "target-cpu"="dav-c310-vec" "target-features"="+ATOMIC,+ArchV130,+AregRedefinable,+ArithmeticBf16,+AtomicForB8 ,+F8e4m3,+F8e5m2,+F8e8m0,+FFTSBlk,+Fp4e1m2x2,+Fp4e2m1x2,+LDExtRefine,+MOVX8,+MSTX,+SPR7bits,+SyncV,+dav-c310-vec" }
attributes #2 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }

!llvm.module.flags = !{!0}
!hivm.annotations = !{!1, !2, !3, !4, !5}

!0 = !{i32 2, !"Debug Info Version", i32 3}
!1 = distinct !{null, !"simt-max-threads", i32 1024}
!2 = distinct !{null, !"simt-max-registers", i32 32}
!3 = !{ptr @mhc_sinkhorn_bwd_ascend_kernel_kernel_mix_aiv, !"kernel", i32 1}
!4 = !{ptr @mhc_sinkhorn_bwd_ascend_kernel_kernel_mix_aiv, !"kernel_with_simd", i32 1}
!5 = !{ptr @mhc_sinkhorn_bwd_ascend_kernel_kernel_mix_aiv, !"kernel_with_simt", i32 1}
!6 = !{!"simt-max-threads", i32 1024}
!7 = !{!"simt-max-registers", i32 32}

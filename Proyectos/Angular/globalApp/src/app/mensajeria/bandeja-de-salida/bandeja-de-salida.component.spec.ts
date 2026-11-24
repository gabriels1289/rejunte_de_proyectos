import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { BandejaDeSalidaComponent } from './bandeja-de-salida.component';

describe('BandejaDeSalidaComponent', () => {
  let component: BandejaDeSalidaComponent;
  let fixture: ComponentFixture<BandejaDeSalidaComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ BandejaDeSalidaComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(BandejaDeSalidaComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});

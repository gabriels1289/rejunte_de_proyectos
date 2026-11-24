import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { RankingYEdicionComponent } from './ranking-y-edicion.component';

describe('RankingYEdicionComponent', () => {
  let component: RankingYEdicionComponent;
  let fixture: ComponentFixture<RankingYEdicionComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ RankingYEdicionComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(RankingYEdicionComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
